#include "easy_acceptor.h"
#include "easy_connection.h"
#include "easy_worker.h"
#include "easy_contextpool.h"
#include "easy_context.h"
#include "easy_network.h"

#ifdef WIN32

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

#endif

#ifdef _LINUX

EasyAcceptor::EasyAcceptor(PSOCKADDR_IN addr, EasyHandler* pHandler)
{
	int32 rc = 0;
	uint32 val = 0;

	epfd_ = epoll_create(256);
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EASY_ASSERT(socket_ != INVALID_SOCKET);
	if (socket_ == INVALID_SOCKET)
	{
		printf("Create acceptor socket failed, err=%d\n", easy_last_error());
		return;
	}

	// set snd buf and recv buf to 0, it's said that it must improve the performance
	// notice!, this two statments will impact performance in linux
	//setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
	//setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

	val = 1;
	setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
	setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

	LOG_STT(_T("Create and configure acceptor socket"));

	ev_.data.fd = socket_;
	ev_.events = EPOLLIN | EPOLLET | EPOLLOUT;
	epoll_ctl(epfd_, EPOLL_CTL_ADD, socket_, &ev_);
	
	// bind acceptor's socket to assigned ip address
	rc = bind(socket_, (sockaddr*)addr, sizeof(*addr));
	EASY_ASSERT(rc == 0);
	if (rc != 0)
	{
		LOG_ERR(_T("bind to address failed, err=%d"), rc);
		return;
	}

	// set socket into listening for incoming connection
	rc = listen(socket_, SOMAXCONN);
	EASY_ASSERT(rc == 0);
	if (rc != 0)
	{
		LOG_ERR(_T("listen to the socket, err=%d"), rc);
		return;
	}

	handler_ = *pHandler;

	LOG_STT(_T("Initialize acceptor success"));
}

EasyAcceptor::~EasyAcceptor()
{
	// close the accept socket
	if (socket_ != INVALID_SOCKET)
	{
		close(socket_);
	}

	LOG_STT(_T("Destroy acceptor success"));
}

void EasyAcceptor::AcceptConnection()
{
	LOG_DBG(_T("ready to accept connection"));
	EasyConnection* pConnection = new EasyConnection(this);
	socklen_t clilen;
	pConnection->socket_ = accept(socket_, (sockaddr*)&pConnection->addr_, &clilen);
	fcntl(pConnection->socket_, F_SETFL, O_NONBLOCK);
	pConnection->ev_.data.ptr = pConnection;
	pConnection->ev_.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd_, EPOLL_CTL_ADD, pConnection->socket_, &pConnection->ev_);
	LOG_DBG(_T("receive a new connection"));

	handler_.OnConnection((ConnID)pConnection);
}

#endif