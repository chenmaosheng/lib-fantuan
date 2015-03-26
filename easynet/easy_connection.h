#ifndef _H_EASY_CONNECTION
#define _H_EASY_CONNECTION

#include "common.h"
#include <semaphore.h>

struct EasyAcceptor;
struct EasyContext;
struct EasyConnection
{
	EasyConnection(EasyAcceptor* pAcceptor);

	int HandleMessage();
	int SendMessage();
	int SendMessage(char* buffer, int len);

	int				socket_;
	sockaddr_in		addr_;
	EasyAcceptor*	acceptor_;
	epoll_event		ev_;
	EasyContext*	recv_context_;
	EasyContext*	send_context_;
	sem_t			sem_;
};

#endif

