#ifndef _H_EASY_ACCEPTOR
#define _H_EASY_ACCEPTOR

#include "easy_handler.h"
#include "easy_context.h"

class EasyWorker;
class EasyContextPool;
#ifdef WIN32
class EasyAcceptor
{
public:
	// initialize the acceptor, but not running at first
	int32 Init(PSOCKADDR_IN addr, EasyWorker* pWorker, EasyContextPool* pContextPool, EasyHandler* pHandler);
	// stop and destroy the acceptor, close all connection
	void Destroy();

	// start the acceptor and ready to receive connection
	void Start();
	// stop receiving connection
	void Stop();
	// post asynchronous accept to receive oncoming connection
	void Accept();

	// set the bind server from app layer
	void SetServer(void*);
	void* GetServer();

	// use these two static functions to create and destroy acceptor
	static EasyAcceptor* CreateAcceptor(PSOCKADDR_IN addr, EasyWorker* pWorker, EasyContextPool* pContextPool, EasyHandler* pHandler);
	static void DestroyAcceptor(EasyAcceptor*);

public:
	SOCKET	socket_;
	EasyHandler	handler_;				// io handler
	EasyWorker*	worker_;				// worker thread
	EasyContextPool* context_pool_;		// related context pool
	void*	server_;				// related server
	EasyContext context_;				// initial context

	LONG	iorefs_;				// reference count to record the number of io, if start, add one, if finished, minus one
	LONG	running_;				// is running or not
	uint32	total_connection_;		// number of connections
	PSLIST_HEADER free_connection_;	// use SList to manage all free connections in order to improve performance
};

#endif

#ifdef _LINUX
class EasyAcceptor
{
public:
	EasyAcceptor(uint32 ip, uint16 port);

	void AcceptConnection();

	int			socket_;
	int			epfd_;
	epoll_event	ev_;
	epoll_event	events_[20];
	EasyConnection* conn_;
};

#endif

#endif

