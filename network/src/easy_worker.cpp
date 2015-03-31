#include "easy_worker.h"
#include "easy_connection.h"
#include "easy_context.h"
#include "easy_acceptor.h"
#include "easy_contextpool.h"

#ifdef WIN32

EasyWorker::EasyWorker(uint32 iThreadCount) : thread_count_(0)
{
	// create iocp handle
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	// create all thread
	while (thread_count_ < iThreadCount)
	{
		std::thread thread_(&EasyWorker::WorkerThread, this);
		++thread_count_;
	}

	LOG_STT(_T("Initialize worker success, thread count=%d"), thread_count_);
}

EasyWorker::~EasyWorker()
{
	// if threads are not all closed, wait for them
	while (thread_count_)
	{
		PostQueuedCompletionStatus(iocp_, 0, 0, NULL);
		Sleep(100);
	}

	LOG_STT(_T("destroy worker success"));

	if (iocp_)
	{
		CloseHandle(iocp_);
	}

	LOG_STT(_T("destroy iocp handle"));
}

uint32 EasyWorker::WorkerThread(PVOID pParam)
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
							pConnection->connected_ = 1;
							if (pConnection->handler_.OnConnection((ConnID)pConnection) && 
								(pContext = pConnection->context_pool_->PopInputContext()))
							{
								// post a receive request
								LOG_DBG(_T("Post a WSARecv after accept"));
								pConnection->AsyncRecv(pContext);
							}
							else
							{
								// post a disconnect request
								LOG_ERR(_T("OnAccept failed"));
								pConnection->AsyncDisconnect();
							}
						}

						InterlockedDecrement(&pAcceptor->iorefs_);
					}
				}
				break;

			case OPERATION_CONNECT:
				{
					if (bResult)
					{
						int32 rc = 0;

						pConnection->connected_ = 1;
						rc = setsockopt(pConnection->socket_, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
						_ASSERT(rc == 0);
						if (rc == 0)
						{
							if (pConnection->handler_.OnConnection((ConnID)pConnection) &&
								(pContext = pConnection->context_pool_->PopInputContext()))
							{
								// post a receive request
								LOG_DBG(_T("Post a WSARecv after connect"));
								pConnection->AsyncRecv(pContext);
							}
							else
							{
								// post a disconnect request
								LOG_ERR(_T("OnConnect failed"));
								pConnection->AsyncDisconnect();
							}
						}
					}
					else
					{
						// invoke when connect failed
						LOG_ERR(_T("OnConnectFailed"));
						pConnection->handler_.OnConnectFailed(pConnection->client_);
						EasyConnection::Close(pConnection);
					}
				}
				break;

			case OPERATION_DISCONNECT:
				{
					LOG_DBG(_T("OnDisconnect, iorefs=%d"), pConnection->iorefs_);
					if (pConnection->iorefs_)
					{
						PostQueuedCompletionStatus(pWorker->iocp_, dwNumRead, key, lpOverlapped);
					}
					else
					{
						pConnection->handler_.OnDisconnect((ConnID)pConnection);
					}
				}
				break;

			case OPERATION_RECV:
				{
					if (dwNumRead == 0)
					{
						// post a disconnect request
						LOG_DBG(_T("Client post a disconnect request"));
						pConnection->context_pool_->PushInputContext(pContext);
						pConnection->AsyncDisconnect();
					}
					else
					{
						pConnection->handler_.OnData((ConnID)pConnection, (uint32)dwNumRead, pContext->buffer_);
						LOG_DBG(_T("Post a WSARecv after recv"));
						pConnection->AsyncRecv(pContext);
					}

					InterlockedDecrement(&pConnection->iorefs_);
				}
				break;

			case OPERATION_SEND:
				{
					pConnection->context_pool_->PushOutputContext(pContext);
					InterlockedDecrement(&pConnection->iorefs_);
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
	std::thread thread_(&EasyWorker::WorkerThread, pAcceptor);
	LOG_STT(_T("Initialize worker success"));
}

void* EasyWorker::WorkerThread(void* ptr)
{
	EasyAcceptor* pAcceptor = (EasyAcceptor*)ptr;
	do
	{
		int nfds = epoll_wait(pAcceptor->epfd_, pAcceptor->events_, MAX_EVENT, 500);
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
					pConnection->RecvData();
				}
				else if (pAcceptor->events_[i].events & EPOLLOUT)
				{
					EasyConnection* pConnection = (EasyConnection*)pAcceptor->events_[i].data.ptr;
					LOG_DBG(_T("trigger, epolllout"));
					pConnection->SendMessage();
				}
			}
		}
	}while (true);

	return NULL;
}

#endif
