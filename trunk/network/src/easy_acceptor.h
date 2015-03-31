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

#endif

#ifdef _LINUX
class EasyAcceptor
{
public:
	EasyAcceptor(PSOCKADDR_IN addr, EasyHandler* pHandler);
	~EasyAcceptor();

	void	AcceptConnection();

	// set the bind server from app layer
	void	SetServer(void* pServer)
	{
		server_ = pServer;
	}
	void*	GetServer()
	{
		return server_;
	}

	int			socket_;
	int			epfd_;
	epoll_event	ev_;
	epoll_event	events_[MAX_EVENT];
	EasyHandler	handler_;				// io handler
	EasyConnection* conn_;
};

#endif

#endif

