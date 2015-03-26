#ifndef _H_EASY_ACCEPTOR
#define _H_EASY_ACCEPTOR

#include "easy_context.h"

class EasyWorker;
class EasyConnection;
class EasyAcceptor
{
public:
#ifdef WIN32
	// initialize the acceptor, but not running at first
	int32 Init(PSOCKADDR_IN addr, EasyWorker* pWorker);
	// stop and destroy the acceptor, close all connection
	void Destroy();

	// start the acceptor and ready to receive connection
	void Start();
	// stop receiving connection
	void Stop();
	// post asynchronous accept to receive oncoming connection
	void Accept();

	// use these two static functions to create and destroy acceptor
	static EasyAcceptor* CreateAcceptor(PSOCKADDR_IN addr, EasyWorker* pWorker);
	static void DestroyAcceptor(EasyAcceptor*);

public:
	SOCKET  socket_;
	EasyWorker* worker_;                                // worker thread
	EasyContext context_;                               // initial context
        
	LONG    running_;                               // is running or not
#endif

#ifdef _LINUX
	EasyAcceptor(unsigned int ip, unsigned short port);

	void AcceptConnection();

	int			socket_;
	int			epfd_;
	epoll_event	ev_;
	epoll_event	events_[20];
	EasyConnection* conn_;
#endif
};

#endif
