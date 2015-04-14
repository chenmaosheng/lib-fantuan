#include "easy_tcp.h"

class EasyNetwork
{
public:
	// intialize
	static int32 Init();
	// destroy
	static void Destroy();

public:
	static LPFN_ACCEPTEX acceptex_;		// asynchorous accept function address
	static LPFN_CONNECTEX connectex_;	// asynchorous connect function address
	static LPFN_DISCONNECTEX disconnectex_; // asynchorous disconnect function address
	static LPFN_GETACCEPTEXSOCKADDRS getacceptexsockaddrs_;	// asynchorous getsockaddr function address
};

LPFN_ACCEPTEX EasyNetwork::acceptex_;
LPFN_CONNECTEX EasyNetwork::connectex_;
LPFN_DISCONNECTEX EasyNetwork::disconnectex_;
LPFN_GETACCEPTEXSOCKADDRS EasyNetwork::getacceptexsockaddrs_;

int32 EasyNetwork::Init()
{
	WSADATA wsd;
	SOCKET s;
	DWORD cb;

	// start WSA
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		LOG_ERR(_T("WSAStartup failed"));
		return -1;
	}

	// create a socket to initialize asynchronous operation function pointer, need to close when finished
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_ASSERT(s != INVALID_SOCKET);
	if (s == INVALID_SOCKET)
	{
		LOG_ERR(_T("Create socket failed"));
		return -2;
	}

	GUID GuidAcceptEx = WSAID_ACCEPTEX, GuidConnectEx = WSAID_CONNECTEX, GuidDisconnectEx = WSAID_DISCONNECTEX, GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &acceptex_, sizeof(acceptex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &connectex_, sizeof(connectex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidDisconnectEx, sizeof(GuidDisconnectEx), &disconnectex_, sizeof(disconnectex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidGetAcceptExSockaddrs, sizeof(GuidGetAcceptExSockaddrs), &getacceptexsockaddrs_, sizeof(getacceptexsockaddrs_), &cb, NULL, NULL);
	closesocket(s);

	LOG_STT(_T("Intialize network success"));
	return 0;
}

void EasyNetwork::Destroy()
{
	WSACleanup();
}

typedef bool (__stdcall* ON_CONNECTION)(ConnID);
typedef void (__stdcall* ON_DISCONNECT)(ConnID);
typedef void (__stdcall* ON_DATA)(ConnID, uint32, char*);
typedef void (__stdcall* ON_CONNECT_FAILED)(void*);

struct EasyHandler
{
	ON_CONNECTION OnConnection;
	ON_DISCONNECT OnDisconnect;
	ON_DATA OnData;
	ON_CONNECT_FAILED OnConnectFailed;
};

enum
{
	INVALID_OPERATION,
	OPERATION_ACCEPT,		// accept operation
	OPERATION_CONNECT,		// connect operation
	OPERATION_DISCONNECT,	// disconnect operation
	OPERATION_RECV,			// receive data operation
	OPERATION_SEND,			// send data operation
};

#define CTXOFFSET	offsetof(EasyContext, overlapped_)	// overlapped_ offset
#define BUFOFFSET	offsetof(EasyContext, buffer_)		// buffer_ offset

struct EasyConnection;
class EasyContextPool;
struct EasyContext 
	: SLIST_ENTRY
{
	WSAOVERLAPPED		overlapped_;		// overlapped io
	WSABUF				wsabuf_;			// overlapped buf
	int32				operation_type_;	// overlapped operation type
	EasyConnection*		connection_;		// context's connection
	EasyContextPool*	context_pool_;		// the pool which is based on

	char				buffer_[(sizeof(SOCKADDR)+16)*2];		// received buffer, at least save 2 sockaddrs(acceptex needed)
};

class EasyContextPool
{
public:
	// initialize context pool with initial input size and output size
	EasyContextPool(uint32 input_buffer_size=MAX_INPUT_BUFFER, uint32 output_buffer_size=MAX_OUTPUT_BUFFER);
	// destroy context pool
	~EasyContextPool();

	// pop a context from input and output pool
	EasyContext*	PopInputContext();
	EasyContext*	PopOutputContext();
	// push a context into input and output pool
	void			PushInputContext(EasyContext*);
	void			PushOutputContext(EasyContext*);

	// pop a context, but only return its buffer
	char*			PopOutputBuffer();
	// push a context, use its buffer to point to the context
	void			PushOutputBuffer(char* buffer);

public:
	uint32			input_buffer_size_;			// input pool size
	uint32			input_context_count_;		// used input count
	uint32			output_buffer_size_;		// output pool size
	uint32			output_context_count_;		// used output count

	SLIST_HEADER	input_context_pool_;	// SList of input pool
	SLIST_HEADER	output_context_pool_;	// SList of output pool
};

class EasyWorker
{
public:
	EasyWorker(uint32 iThreadCount=2);
	~EasyWorker();

	static uint32 WINAPI WorkerThread(void*);

public:
	HANDLE  iocp_;			// binded iocp handle
	uint32  thread_count_;	// the number of threads
};

class EasyAcceptor
{
public:
	// initialize the acceptor, but not running at first
	EasyAcceptor(PSOCKADDR_IN addr, EasyWorker* pWorker, EasyContextPool* pContextPool, EasyHandler* pHandler);
	// stop and destroy the acceptor, close all connection
	~EasyAcceptor();

	// start the acceptor and ready to receive connection
	void				Start();
	// stop receiving connection
	void				Stop();
	// post asynchronous accept to receive oncoming connection
	void				Accept();

	// set the bind server from app layer
	void				SetServer(void* pServer)
	{
		server_ = pServer;
	}
	void*				GetServer()
	{
		return server_;
	}

public:
	SOCKET				socket_;
	EasyHandler			handler_;			// io handler
	EasyWorker*			worker_;			// worker thread
	EasyContextPool*	context_pool_;		// related context pool
	void*				server_;			// related server
	EasyContext			context_;			// initial context

	LONG				iorefs_;			// reference count to record the number of io, if start, add one, if finished, minus one
	LONG				running_;			// is running or not
	uint32				total_connection_;	// number of connections
	PSLIST_HEADER		free_connection_;	// use SList to manage all free connections in order to improve performance
};

struct EasyConnection : SLIST_ENTRY
{
	SOCKET				socket_;	
	SOCKADDR_IN			sockaddr_;		// connection's address
	EasyHandler			handler_;		// io handler
	EasyContextPool*	context_pool_;	// point to context pool
	EasyWorker*			worker_;
	EasyAcceptor*		acceptor_;		// related acceptor
	EasyContext*		context_;		// initial context
	void*				client_;		// pointer from app layer

	LONG				iorefs_;		// io reference counter
	LONG				connected_;		// is connected
	LONG				iorefmax_;		// max io reference allowed

	// asynchronous connect
	bool				AsyncConnect(PSOCKADDR_IN addr, void* client);
	// asynchronous disconnect
	void				AsyncDisconnect();
	// asynchronous send, need pop a context first
	void				AsyncSend(EasyContext*);
	// asynchronous receive, need pop a context first
	void				AsyncRecv(EasyContext*);
	void				AsyncSend(uint32 len, char* buf);

	inline void			SetClient(void* pClient)
	{
		client_ = pClient;
	}
	inline void*		GetClient() const
	{
		return client_;
	}
	inline void			SetRefMax(uint16 iMax)
	{
		iorefmax_ = iMax;
	}
	inline bool			IsConnected() const
	{
		return connected_ ? true : false;
	}

	// static function to create and close
	static EasyConnection*	Create(EasyHandler* pHandler, EasyContextPool* pContextPool, EasyWorker* pWorker, EasyAcceptor* pAcceptor);
	static bool				Connect(PSOCKADDR_IN pAddr, EasyHandler* pHandler, EasyContextPool* pContextPool, EasyWorker* pWorker, void* pClient);
	static void				Close(EasyConnection*);	// attention: don't call this function if disconnect not called
	static void				Delete(EasyConnection*);
};

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

EasyAcceptor::EasyAcceptor(PSOCKADDR_IN addr, EasyWorker* pWorker, EasyContextPool* pContextPool, EasyHandler* pHandler)
{
	int32 rc = 0;
	DWORD val = 0;

	iorefs_ = 0;
	running_ = 0;
	total_connection_ = 0;

	// initialize acceptor's tcp socket
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EASY_ASSERT(socket_ != INVALID_SOCKET);
	if (socket_ == INVALID_SOCKET)
	{
		printf("Create acceptor socket failed, err=%d\n", easy_last_error());
		return;
	}

	// set snd buf and recv buf to 0, it's said that it must improve the performance
	setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
	setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

	val = 1;
	setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
	setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

	LOG_STT(_T("Create and configure acceptor socket"));

	// create slist of free connection
	free_connection_ = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT);
	_ASSERT(free_connection_);
	if (!free_connection_)
	{
		LOG_ERR(_T("Allocate SList of free connection failed, err=%d"), GetLastError());
		return;
	}

	InitializeSListHead(free_connection_);

	LOG_STT(_T("Create and initialize SList of free connection"));

	// initialize the context and set io type to accept
	ZeroMemory(&context_.overlapped_, sizeof(WSAOVERLAPPED));
	context_.operation_type_ = OPERATION_ACCEPT;

	// bind acceptor's socket to iocp handle
	if (!CreateIoCompletionPort((HANDLE)socket_, pWorker->iocp_, (ULONG_PTR)this, 0))
	{
		_ASSERT(false && _T("CreateIoCompletionPort failed"));
		LOG_ERR(_T("CreateIoCompletionPort failed, err=%d"), WSAGetLastError());
		return;
	}

	// bind acceptor's socket to assigned ip address
	rc = bind(socket_, (sockaddr*)addr, sizeof(*addr));
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		LOG_ERR(_T("bind to address failed, err=%d"), rc);
		return;
	}

	// set socket into listening for incoming connection
	rc = listen(socket_, SOMAXCONN);
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		LOG_ERR(_T("listen to the socket, err=%d"), rc);
		return;
	}

	worker_ = pWorker;
	handler_ = *pHandler;
	context_pool_ = pContextPool;
	server_ = NULL;

	LOG_STT(_T("Initialize acceptor success"));
}

EasyAcceptor::~EasyAcceptor()
{
	// first stop the acceptor
	Stop();

	// if some io are not released, wait a while
	while (iorefs_)
	{
		Sleep(100);
	}

	// clear all connections in free list
	if (free_connection_)
	{
		while (QueryDepthSList(free_connection_) != total_connection_)
		{
			Sleep(100);
		}

		while (QueryDepthSList(free_connection_))
		{
			EasyConnection::Close((EasyConnection*)InterlockedPopEntrySList(free_connection_));
		}

		_aligned_free(free_connection_);
	}

	LOG_STT(_T("Clear all connections in free list"));

	// close the accept socket
	if (socket_ != INVALID_SOCKET)
	{
		closesocket(socket_);
	}

	LOG_STT(_T("Destroy acceptor success"));
}

void EasyAcceptor::Accept()
{
	// get one connection from free list
	EasyConnection* pConnection = (EasyConnection*)InterlockedPopEntrySList(free_connection_);
	if (!pConnection)
	{
		pConnection = EasyConnection::Create(&handler_, context_pool_, worker_, this);
		_ASSERT(pConnection);
		if (!pConnection)
		{
			LOG_ERR(_T("Create connection failed, err=%d"), GetLastError());
			running_ = 0;
			return;
		}
	}
	else
	{
		total_connection_--;
	}

	LOG_DBG(_T("Get a new connection and wait for incoming connect"));

	pConnection->client_ = NULL;
	context_.connection_ = pConnection;

	InterlockedIncrement(&iorefs_);
	DWORD dwXfer;

	// post an asychronous accept
	if (!EasyNetwork::acceptex_(socket_, pConnection->socket_, context_.buffer_, 0, sizeof(SOCKADDR_IN)+16,
		sizeof(SOCKADDR_IN)+16, &dwXfer, &context_.overlapped_))
	{
		int32 iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("AcceptEx failed, err=%d"), iLastError);

			EasyConnection::Close(pConnection);
			InterlockedPushEntrySList(free_connection_, pConnection);
			total_connection_++;
			InterlockedDecrement(&iorefs_);
			running_ = 0;
		}
		else
		{
			LOG_DBG(_T("Acceptex pending"));
		}
	}

	LOG_DBG(_T("AcceptEx success"));
}

void EasyAcceptor::Start()
{
	// check if running is not 0
	if (!InterlockedCompareExchange(&running_, 1, 0))
	{
		// confirm there is no uncomplete io request
		while (iorefs_)
		{
			Sleep(100);
		}

		Accept();
	}
}

void EasyAcceptor::Stop()
{
	// set running to 0
	running_ = 0;
}

EasyWorker::EasyWorker(uint32 iThreadCount) : thread_count_(0)
{
	// create iocp handle
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	// create all thread
	while (thread_count_ < iThreadCount)
	{
		uintptr_t thread_ = _beginthreadex(NULL, 0,  &EasyWorker::WorkerThread, this, 0, NULL);
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

EasyContextPool::EasyContextPool(uint32 input_buffer_size, uint32 output_buffer_size)
{
	input_buffer_size_ = input_buffer_size;
	output_buffer_size_ = output_buffer_size;
	input_context_count_ = 0;
	output_context_count_ = 0;

	// initialize each Slist
	InitializeSListHead(&input_context_pool_);
	InitializeSListHead(&output_context_pool_);

	LOG_STT(_T("Initialize context pool success"));
}

EasyContextPool::~EasyContextPool()
{
	while (QueryDepthSList(&input_context_pool_) != input_context_count_)
	{
		Sleep(100);
	}

	while (QueryDepthSList(&input_context_pool_))
	{
		_aligned_free(InterlockedPopEntrySList(&input_context_pool_));
	}

	while (QueryDepthSList(&output_context_pool_) != output_context_count_)
	{
		Sleep(100);
	}

	while (QueryDepthSList(&output_context_pool_))
	{
		_aligned_free(InterlockedPopEntrySList(&output_context_pool_));
	}

	LOG_STT(_T("Destroy context pool success\n"));
}

EasyContext* EasyContextPool::PopInputContext()
{
	EasyContext* pContext = (EasyContext*)InterlockedPopEntrySList(&input_context_pool_);
	if (!pContext)
	{
		pContext = (EasyContext*)_aligned_malloc(sizeof(EasyContext)+input_buffer_size_, MEMORY_ALLOCATION_ALIGNMENT);
		_ASSERT(pContext);
		if (!pContext)
		{
			LOG_ERR(_T("Allocate context failed, err=%d"), GetLastError());
			return NULL;
		}

		ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
		pContext->operation_type_ = OPERATION_RECV;
		pContext->context_pool_ = this;
		pContext->wsabuf_.buf = (char*)pContext->buffer_;
		InterlockedIncrement((LONG*)&input_context_count_);
	}

	return pContext;
}

EasyContext* EasyContextPool::PopOutputContext()
{
	EasyContext* pContext = (EasyContext*)InterlockedPopEntrySList(&output_context_pool_);
	if (!pContext)
	{
		pContext = (EasyContext*)_aligned_malloc(sizeof(EasyContext)+output_buffer_size_, MEMORY_ALLOCATION_ALIGNMENT);
		_ASSERT(pContext);
		if (!pContext)
		{
			LOG_ERR(_T("Allocate context failed, err=%d"), GetLastError());
			return NULL;
		}

		ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
		pContext->operation_type_ = OPERATION_SEND;
		pContext->context_pool_ = this;
		pContext->wsabuf_.buf = (char*)pContext->buffer_;
		InterlockedIncrement((LONG*)&output_context_count_);
	}

	return pContext;
}

void EasyContextPool::PushInputContext(EasyContext* pContext)
{
	InterlockedPushEntrySList(&input_context_pool_, pContext);
}

void EasyContextPool::PushOutputContext(EasyContext* pContext)
{
	InterlockedPushEntrySList(&output_context_pool_, pContext);
}

char* EasyContextPool::PopOutputBuffer()
{
	EasyContext* pContext = PopOutputContext();
	_ASSERT(pContext);
	if (pContext)
	{
		return pContext->buffer_;
	}

	LOG_DBG(_T("Pop a buffer from pool failed, err=%d"), GetLastError());

	return NULL;
}

void EasyContextPool::PushOutputBuffer(char* buffer)
{
	PushOutputContext((EasyContext*)((char*)buffer - BUFOFFSET));
}

void InitEasyTCP()
{
	EasyLog::Instance()->Init();
	EasyNetwork::Init();
}

void DestroyEasyTCP()
{
	EasyNetwork::Destroy();
}

void* CreateAcceptor(uint32 ip, uint16 port, uint32 count, bool (__stdcall* a)(ConnID), void (__stdcall* b)(ConnID), void (__stdcall* c)(ConnID, uint32, char*), void (__stdcall* d)(void*))
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);

	EasyHandler* handler = new EasyHandler;
	handler->OnConnection = a;
	handler->OnDisconnect = b;
	handler->OnData = c;
	handler->OnConnectFailed = d;

	return new EasyAcceptor(&addr, new EasyWorker(count), new EasyContextPool, handler);
}

void StartAcceptor(void* pAcceptor)
{
	((EasyAcceptor*)pAcceptor)->Start();
}