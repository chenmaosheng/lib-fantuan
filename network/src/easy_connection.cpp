#include "easy_connection.h"
#include "easy_worker.h"
#include "easy_context.h"
#include "easy_contextpool.h"
#include "easy_acceptor.h"
#include "easy_network.h"

#ifdef WIN32

bool EasyConnection::AsyncConnect(PSOCKADDR_IN addr, void* client)
{
	int32 rc = 0;
	client_ = client;
	_ASSERT(context_);
	if (!context_)
	{
		return false;
	}

	// post an asychronous connect
	context_->operation_type_ = OPERATION_CONNECT;
	rc = EasyNetwork::connectex_(socket_, (sockaddr*)addr, sizeof(*addr), NULL, 0, NULL, &context_->overlapped_);
	if (rc == 0)
	{
		int32 iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("ConnectEx failed, err=%d"), iLastError);
			return false;
		}
		else
		{
			LOG_DBG(_T("ConnectEx pending"));
		}
	}

	LOG_DBG(_T("ConnectEx success"));

	return true;
}

void EasyConnection::AsyncDisconnect()
{
	int32 rc = 0;
	// check if is connected
	if (InterlockedCompareExchange(&connected_, 0, 1))
	{
		// post an asychronous disconnect
		context_->operation_type_ = OPERATION_DISCONNECT;
		rc = EasyNetwork::disconnectex_(socket_, &context_->overlapped_, acceptor_ ? TF_REUSE_SOCKET: 0, 0);
		if (rc == 0)
		{
			int32 iLastError = WSAGetLastError();
			_ASSERT(iLastError == ERROR_IO_PENDING);
			if (iLastError != ERROR_IO_PENDING)
			{
				LOG_ERR(_T("DisconnectEx failed, err=%d"), iLastError);
				return;
			}
			else
			{
				LOG_DBG(_T("DisconnectEx pending"));
			}
		}

		LOG_DBG(_T("DisconnectEx success"));
	}
}

void EasyConnection::AsyncSend(EasyContext* pContext)
{
	pContext->connection_ = this;
	// check if reference count is more than max count
	_ASSERT(iorefs_ <= iorefmax_);
	if (iorefs_ > iorefmax_)
	{
		LOG_ERR(_T("reference count is more than max, iorefs=%d"), iorefs_);
		context_pool_->PushOutputContext(pContext);
		AsyncDisconnect();
		return;
	}

	InterlockedIncrement(&iorefs_);
	DWORD dwXfer;
	// post an asychronous send
	if (WSASend(socket_, &pContext->wsabuf_, 1, &dwXfer, 0, &pContext->overlapped_, NULL) == SOCKET_ERROR)
	{
		int32 iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("WSASend failed, err=%d"), iLastError);
			context_pool_->PushOutputContext(pContext);
			AsyncDisconnect();
			InterlockedDecrement(&iorefs_);
			return;
		}
		else
		{
			LOG_DBG(_T("WSASend Pending"));
		}
	}
}

void EasyConnection::AsyncRecv(EasyContext* pContext)
{
	pContext->connection_ = this;
	pContext->wsabuf_.len = context_pool_->input_buffer_size_;
	InterlockedIncrement(&iorefs_);
	DWORD dwXfer, dwFlag = 0;
	// post an asychronous receive
	if (WSARecv(socket_, &pContext->wsabuf_, 1, &dwXfer, &dwFlag, &pContext->overlapped_, NULL) == SOCKET_ERROR)
	{
		int32 iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("WSARecv failed, err=%d"), iLastError);
			context_pool_->PushInputContext(pContext);
			AsyncDisconnect();
			InterlockedDecrement(&iorefs_);
			return;
		}
		else
		{
			LOG_DBG(_T("WSARecv Pending"));
		}
	}
}

void EasyConnection::AsyncSend(uint32 len, char* buf)
{
	EasyContext* pContext = (EasyContext*)((char*)buf - BUFOFFSET);
	_ASSERT(pContext->operation_type_ == OPERATION_SEND);
	if (pContext->operation_type_ != OPERATION_SEND)
	{
		LOG_ERR(_T("Operation type exception, type=%d"), pContext->operation_type_);
		return;
	}

	if (pContext->context_pool_->output_buffer_size_ < len)
	{
		LOG_ERR(_T("length is oversize, length=%d"), len);
		return;
	}

	pContext->wsabuf_.len = len;
	AsyncSend(pContext);
}

void EasyConnection::SetClient(void* pClient)
{
	client_ = pClient;
}

void* EasyConnection::GetClient()
{
	return client_;
}

void EasyConnection::SetRefMax(uint16 iMax)
{
	iorefmax_ = iMax;
}

bool EasyConnection::IsConnected()
{
	return connected_ ? true : false;
}

EasyConnection* EasyConnection::Create(EasyHandler* pHandler, EasyContextPool* pContextPool, EasyWorker* pWorker, EasyAcceptor* pAcceptor)
{
	EasyConnection* pConnection = (EasyConnection*)_aligned_malloc(sizeof(EasyConnection), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnection)
	{
		// initialize connection's tcp socket
		pConnection->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		_ASSERT(pConnection->socket_ != INVALID_SOCKET);
		if (pConnection->socket_ != INVALID_SOCKET)
		{
			// the 3rd param is the key of getqueued
			if (CreateIoCompletionPort((HANDLE)pConnection->socket_, pWorker->iocp_, (ULONG_PTR)pConnection, 0))
			{
				DWORD val = 0;

				// set snd buf and recv buf to 0, it's said that it must improve the performance
				setsockopt(pConnection->socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
				setsockopt(pConnection->socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

				val = 1;
				setsockopt(pConnection->socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
				setsockopt(pConnection->socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

				pConnection->context_ = (EasyContext*)_aligned_malloc(sizeof(EasyContext), MEMORY_ALLOCATION_ALIGNMENT);
				_ASSERT(pConnection->context_);
				if (pConnection->context_)
				{
					pConnection->handler_ = *pHandler;
					pConnection->context_pool_ = pContextPool;
					pConnection->worker_ = pWorker;
					pConnection->acceptor_ = pAcceptor;
					pConnection->context_->connection_ = pConnection;
					pConnection->connected_ = 0;
					pConnection->iorefs_ = 0;
					pConnection->iorefmax_ = 65536;
					ZeroMemory(&pConnection->context_->overlapped_, sizeof(WSAOVERLAPPED));
					if (!pAcceptor)
					{
						// if acceptor=NULL, means it's called at client side
						// connection's socket must bind to it's address
						SOCKADDR_IN addr;
						ZeroMemory(&addr, sizeof(addr));
						addr.sin_family = AF_INET;
						if (bind(pConnection->socket_, (sockaddr*)&addr, sizeof(addr)) == 0)
						{
							LOG_DBG(_T("Create and configure connection in client side"));
							return pConnection;
						}
						else
						{
							_ASSERT( false && _T("Bind error") );
						}
					}
					else
					{
						LOG_DBG(_T("Create and configure connection in server side"));
						return pConnection;
					}
				}
			}
			else
			{
				_ASSERT(false && _T("CreateIoCompletionPort failed"));
				LOG_ERR(_T("CreateIoCompletionPort failed, err=%d"), WSAGetLastError());
				closesocket(pConnection->socket_);
			}
		}

		_aligned_free(pConnection);
	}

	LOG_ERR(_T("Create connection failed, err=%d"), GetLastError());

	return NULL;
}

bool EasyConnection::Connect(PSOCKADDR_IN pAddr, EasyHandler* pHandler, EasyContextPool* pContextPool, EasyWorker* pWorker, void* pClient)
{
	EasyConnection* pConnection = Create(pHandler, pContextPool, pWorker, NULL);
	_ASSERT(pConnection);
	if (pConnection)
	{
		if (pConnection->AsyncConnect(pAddr, pClient))
		{
			return true;
		}

		Delete(pConnection);
	}

	return false;
}

void EasyConnection::Close(EasyConnection* pConnection)
{
	// check if io reference count is 0
	_ASSERT(pConnection->iorefs_ == 0 && pConnection->connected_ == 0);
	if (pConnection->iorefs_ || pConnection->connected_)
	{
		LOG_ERR(_T("Connection can't be closed, ioref_=%d"), pConnection->iorefs_);
		return;
	}

	// different activity in server and client side
	if (pConnection->acceptor_)
	{
		InterlockedPushEntrySList(pConnection->acceptor_->free_connection_, pConnection);
	}
	else
	{
		Delete(pConnection);
	}

	LOG_DBG(_T("Close connection success"));
}

void EasyConnection::Delete(EasyConnection* pConnection)
{
	closesocket(pConnection->socket_);
	_aligned_free(pConnection->context_);
	_aligned_free(pConnection);

	LOG_DBG(_T("Close socket success"));
}

#endif

#ifdef _LINUX
EasyConnection::EasyConnection(EasyAcceptor* pAcceptor) : acceptor_(pAcceptor)
{
	recv_context_ = new EasyContext;
	recv_context_->buffer_ = (char*)malloc(1024);
	send_context_ = new EasyContext;
	sem_init(&sem_, 0, 1);
}

int EasyConnection::HandleMessage()
{
	bool bReadOK = false;
	int recvNum = 0;
	printf("start to receve message\n");
	recv_context_->buffer_[0] = '\n';
	recv_context_->len_ = 0;
	while (true)
	{
		recvNum = recv(socket_, recv_context_->buffer_ + recvNum, MAXLINE, 0);
		if (recvNum < 0)
		{
			if (errno == EAGAIN)
			{
				printf("recv eagain\n");
				bReadOK = true;
				break;
			}
			else if (errno == EINTR)
			{
				printf("recv eintr\n");
				continue;
			}
			else
			{
				printf("other recv error\n");
				break;
			}
		}
		else if (recvNum == 0)
		{
			printf("close and leave\n");
			close(socket_);
			break;
		}

		recv_context_->len_ += recvNum;
		if (recvNum == MAXLINE)
		{
			printf("hit maxline, continue\n");
			continue;
		}
		else
		{
			bReadOK = true;
			break;
		}
	}

	if (bReadOK)
	{
		recv_context_->buffer_[recv_context_->len_] = '\0';
		printf("message=%s, len=%d\n", recv_context_->buffer_, recv_context_->len_);
		acceptor_->handler_.OnData((ConnID)this, recv_context_->len_, recv_context_->buffer_);
	}

	return recv_context_->len_;
}

int EasyConnection::SendMessage()
{
	if (send_context_->len_ == 0)
	{
		return 0;
	}
	printf("send the left\n");
	int writeNum = 0;
	int total = send_context_->len_;
	char* p = send_context_->buffer_;
	bool bWriteOK = false;
	while (true)
	{
		writeNum = send(socket_, p, total, 0);
		printf("send len=%d\n", total);
		if (writeNum == -1)
		{
			if (errno == EAGAIN)
			{
				printf("send eagain\n");
				bWriteOK = true;
				break;
			}
			else if (errno == EINTR)
			{
				printf("send eintr\n");
				continue;
			}
			else
			{
				printf("other send errors\n");
				break;
			}
		}
		else if (writeNum == 0)
		{
			printf("send close and leave\n");
			close(socket_);
			break;
		}

		if (writeNum == total)
		{
			printf("send all\n");
			bWriteOK = true;
			break;
		}

		printf("send partially\n");

		total -= writeNum;
		p += writeNum;
	}

	if (bWriteOK)
	{
		send_context_->buffer_[0] = '\0';
		send_context_->len_ = 0;
		ev_.events = EPOLLIN | EPOLLET;
		epoll_ctl(acceptor_->epfd_, EPOLL_CTL_MOD, socket_, &ev_);
	}

	sem_post(&sem_);

	return 0;
}

int EasyConnection::SendMessage(char* buffer, int len)
{
	sem_wait(&sem_);
	bool bWriteOK = false;
	int writeNum = 0;
	send_context_->buffer_ = buffer;
	send_context_->len_ = len;
	printf("sendmessage len=%d\n", send_context_->len_);

	while (true)
	{
		writeNum = send(socket_, send_context_->buffer_, send_context_->len_, 0);
		if (writeNum == -1)
		{
			if (errno == EAGAIN)
			{
				printf("sendmessage, eagain, go\n");
				ev_.events = EPOLLOUT | EPOLLET;
				epoll_ctl(acceptor_->epfd_, EPOLL_CTL_MOD, socket_, &ev_);
				return 0;
			}
		}
		else if (writeNum == 0)
		{
			printf("sendmessage close and leave\n");
			close(socket_);
			break;
		}

		if (writeNum == send_context_->len_)
		{
			printf("sendmessage all\n");
			bWriteOK = true;
			break;
		}
		printf("sendmessage partially\n");

		send_context_->len_ -= writeNum;
		send_context_->buffer_ += writeNum;
		printf("sendmessage len=%d\n", send_context_->len_);
	}

	if (bWriteOK)
	{
		send_context_->buffer_[0] = '\0';
		send_context_->len_ = 0;
	}

	sem_post(&sem_);

	return 0;
}

void EasyConnection::SetClient(void* pClient)
{
	client_ = pClient;
}

void* EasyConnection::GetClient()
{
	return client_;
}

void EasyConnection::Close(EasyConnection* pConnection)
{
	close(pConnection->socket_);
}

#endif
