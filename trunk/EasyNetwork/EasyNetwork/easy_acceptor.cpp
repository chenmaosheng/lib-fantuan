#include "easy_acceptor.h"
#include "easy_connection.h"
#include "easy_worker.h"
#include "easy_context.h"
#include "easy_network.h"

#ifdef WIN32
int32 EasyAcceptor::Init(uint32 ip, uint16 port, EasyWorker* pWorker)
{
	int32 rc = 0;
	DWORD val = 0;
	
	running_ = 0;
	
	// initialize acceptor's tcp socket
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_ASSERT(socket_ != INVALID_SOCKET);
	if (socket_ == INVALID_SOCKET)
	{
		printf("Create acceptor socket failed, err=%d\n", WSAGetLastError());
		return -1;
	}

	// set snd buf and recv buf to 0, it's said that it must improve the performance
	setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
	setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

	val = 1;
	setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
	setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

	printf("Create and configure acceptor socket\n");

	// initialize the context and set io type to accept
	ZeroMemory(&context_.overlapped_, sizeof(WSAOVERLAPPED));
	context_.operation_type_ = OPERATION_ACCEPT;

	// bind acceptor's socket to iocp handle
	if (!CreateIoCompletionPort((HANDLE)socket_, pWorker->iocp_, (ULONG_PTR)this, 0))
	{
		_ASSERT(false && "CreateIoCompletionPort failed");
		printf("CreateIoCompletionPort failed, err=%d\n", WSAGetLastError());
		return -4;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);

	// bind acceptor's socket to assigned ip address
	rc = bind(socket_, (sockaddr*)&addr, sizeof(addr));
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		printf("bind to address failed, err=%d\n", rc);
		return -5;
	}

	// set socket into listening for incoming connection
	rc = listen(socket_, SOMAXCONN);
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		printf("listen to the socket, err=%d\n", rc);
		return -6;
	}

	worker_ = pWorker;
	
	printf("Initialize acceptor success\n");

	return 0;
}

void EasyAcceptor::Destroy()
{
	// first stop the acceptor
	Stop();

	// todo: io may not fully handled

	printf("Clear all connections in free list\n");

	// close the accept socket
	if (socket_ != INVALID_SOCKET)
	{
		closesocket(socket_);
	}

	printf("Destroy acceptor success\n");
}

void EasyAcceptor::Accept()
{
	EasyConnection* pConnection = EasyConnection::Create(worker_, this);
	printf("Get a new connection and wait for incoming connect\n");

	context_.connection_ = pConnection;

	DWORD dwXfer;

	// post an asychronous accept
	if (!EasyNetwork::acceptex_(socket_, pConnection->socket_, context_.buffer_, 0, sizeof(SOCKADDR_IN)+16,
		sizeof(SOCKADDR_IN)+16, &dwXfer, &context_.overlapped_))
	{
		int32 iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			printf("AcceptEx failed, err=%d\n", iLastError);

			EasyConnection::Close(pConnection);
			running_ = 0;
		}
		else
		{
			printf("Acceptex pending\n");
		}
	}

	printf("AcceptEx success\n");
}

void EasyAcceptor::Start()
{
	// check if running is not 0
	if (!InterlockedCompareExchange(&running_, 1, 0))
	{
		Accept();
	}
}

void EasyAcceptor::Stop()
{
	// set running to 0
	running_ = 0;
}

EasyAcceptor* EasyAcceptor::CreateAcceptor(uint32 ip, uint16 port, EasyWorker* pWorker)
{
	EasyAcceptor* pAcceptor = (EasyAcceptor*)_aligned_malloc(sizeof(EasyAcceptor), MEMORY_ALLOCATION_ALIGNMENT);
	if (pAcceptor)
	{
		pAcceptor->Init(ip, port, pWorker);
	}

	return pAcceptor;
}

void EasyAcceptor::DestroyAcceptor(EasyAcceptor* pAcceptor)
{
	pAcceptor->Destroy();
	_aligned_free(pAcceptor);
}
#endif

#ifdef _LINUX

EasyAcceptor::EasyAcceptor(unsigned int ip, unsigned short port)
{
	epfd_ = epoll_create(256);
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ev_.data.fd = socket_;
	ev_.events = EPOLLIN | EPOLLET | EPOLLOUT;
	epoll_ctl(epfd_, EPOLL_CTL_ADD, socket_, &ev_);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	bind(socket_, (sockaddr*)&addr, sizeof(addr));
	listen(socket_, SOMAXCONN);
}

void EasyAcceptor::AcceptConnection()
{
	printf("ready to accept connection\n");
	EasyConnection* pConnection = new EasyConnection(this);
	printf("new connection\n");
	socklen_t clilen;
	pConnection->socket_ = accept(socket_, (sockaddr*)&pConnection->addr_, &clilen);
	fcntl(pConnection->socket_, F_SETFL, O_NONBLOCK);
	printf("set to nonblock\n");
	pConnection->ev_.data.ptr = pConnection;
	pConnection->ev_.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd_, EPOLL_CTL_ADD, pConnection->socket_, &pConnection->ev_);
	printf("receive a new connection\n");
}

#endif
