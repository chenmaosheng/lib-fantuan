#include "easy_worker.h"
#include "easy_connection.h"
#include "easy_context.h"
#include "easy_acceptor.h"

#ifdef WIN32
void EasyWorker::Init(uint32 iThreadCount)
{
	thread_count_ = 0;
	// create iocp handle
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	// create all thread
	while (thread_count_ < iThreadCount)
	{
		HANDLE hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &EasyWorker::WorkerThread, this, 0, NULL);
		CloseHandle(hWorkerThread);

		++thread_count_;
	}

	printf("Initialize worker success, thread count=%d\n", thread_count_);
}

void EasyWorker::Destroy()
{
	// if threads are not all closed, wait for them
	while (thread_count_)
	{
		PostQueuedCompletionStatus(iocp_, 0, 0, NULL);
		Sleep(100);
	}

	printf("destroy worker success\n");

	if (iocp_)
	{
		CloseHandle(iocp_);
	}

	printf("destroy iocp handle\n");
}

EasyWorker* EasyWorker::CreateWorker(uint32 iCount)
{
	EasyWorker* pWorker = (EasyWorker*)_aligned_malloc(sizeof(EasyWorker), MEMORY_ALLOCATION_ALIGNMENT);
	if (pWorker)
	{
		pWorker->Init(iCount);
	}

	return pWorker;
}

void EasyWorker::DestroyWorker(EasyWorker* pWorker)
{
	pWorker->Destroy();
	_aligned_free(pWorker);
}

// todo: pContext may leak except "Accept"
uint32 WINAPI EasyWorker::WorkerThread(PVOID pParam)
{
	BOOL bResult;
	DWORD dwNumRead;
	LPOVERLAPPED lpOverlapped = NULL;
	EasyContext* pContext = NULL;
	ULONG_PTR key;
	EasyConnection* pConnection = NULL;
	EasyAcceptor* pAcceptor = NULL;

	EasyWorker* pWorker = (EasyWorker*)pParam;

	do
	{
		// get io response from iocp
		bResult = GetQueuedCompletionStatus(pWorker->iocp_, &dwNumRead, &key, &lpOverlapped, INFINITE);
		if (lpOverlapped)
		{
			pContext = (EasyContext*)((char*)lpOverlapped - CTXOFFSET);
			pConnection = pContext->connection_;
			switch(pContext->operation_type_)
			{
			case OPERATION_ACCEPT:
				{
					pAcceptor = pConnection->acceptor_;
					if (bResult)
					{
						int32 rc = 0;
						// post another accept request
						pAcceptor->Accept();
						rc = setsockopt(pConnection->socket_, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*)&pAcceptor->socket_, sizeof(pAcceptor->socket_));
						_ASSERT(rc == 0);
						if (rc == 0)
						{
							// confirm connected and invoke handler
							// post a receive request
							printf("Post a WSARecv after accept\n");
							pConnection->AsyncRecv(EasyContext::CreateContext(OPERATION_RECV));
							
						}
					}
				}
				break;

			case OPERATION_CONNECT:
				{
					if (bResult)
					{
						int32 rc = 0;
						
						rc = setsockopt(pConnection->socket_, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
						_ASSERT(rc == 0);
						if (rc == 0)
						{
							// post a receive request
							printf("Post a WSARecv after connect\n");
							pConnection->AsyncRecv(EasyContext::CreateContext(OPERATION_RECV));
						}
					}
					else
					{
						// invoke when connect failed
						printf("OnConnectFailed\n");
						EasyConnection::Close(pConnection);
					}
				}
				break;

			case OPERATION_DISCONNECT:
				{
				}
				break;

			case OPERATION_RECV:
				{
					if (dwNumRead == 0)
					{
						// post a disconnect request
						printf("Client post a disconnect request\n");
						_aligned_free(pContext);
						pConnection->AsyncDisconnect();
					}
					else
					{
						printf("Post a WSARecv after recv\n");
						pConnection->AsyncRecv(pContext);
					}
				}
				break;

			case OPERATION_SEND:
				{
					_aligned_free(pContext);
				}
				break;
			default:
				break;
			}
		}
		else
		{
			break;
		}
	}while(true);

	InterlockedDecrement((LONG*)&pWorker->thread_count_);
	return 0;
}

#endif

#ifdef _LINUX

EasyWorker::EasyWorker(EasyAcceptor* pAcceptor)
{
	pthread_create(&thread_, NULL, &EasyWorker::WorkerThread, pAcceptor);
}

void* EasyWorker::WorkerThread(void* ptr)
{
	EasyAcceptor* pAcceptor = (EasyAcceptor*)ptr;
	do
	{
		int nfds = epoll_wait(pAcceptor->epfd_, pAcceptor->events_, 20, 500);
		for (int i = 0; i < nfds; ++i)
		{
			if (pAcceptor->events_[i].data.fd == pAcceptor->socket_)
			{
				pAcceptor->AcceptConnection();
			}
			else
			{
				if (pAcceptor->events_[i].events & EPOLLIN)
				{
					EasyConnection* pConnection = (EasyConnection*)pAcceptor->events_[i].data.ptr;
					pAcceptor->conn_ = pConnection;
					pConnection->HandleMessage();
				}
				else if (pAcceptor->events_[i].events & EPOLLOUT)
				{
					EasyConnection* pConnection = (EasyConnection*)pAcceptor->events_[i].data.ptr;
					printf("trigger, epolllout\n");
					pConnection->SendMessage();
				}
			}
		}
	}while (true);
}

#endif
