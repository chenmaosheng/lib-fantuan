#ifndef _H_EASY_ACCEPTOR
#define _H_EASY_ACCEPTOR

#include "common.h"

struct EasyConnection;
struct EasyAcceptor
{
	EasyAcceptor(unsigned int ip, unsigned short port);

	void AcceptConnection();

	int			socket_;
	int			epfd_;
	epoll_event	ev_;
	epoll_event	events_[20];
	EasyConnection* conn_;
};

#endif
