#ifndef _H_EASY_CONNECTION
#define _H_EASY_CONNECTION

#include "common.h"

struct EasyAcceptor;
struct EasyConnection
{
	EasyConnection(EasyAcceptor* pAcceptor) : acceptor_(pAcceptor)
	{
		len_ = 0;
		buffer_[0] = '\0';
		pConnection->recv_context_ = new EasyContext(OPERATION_RECV, this);
		pConnection->send_context_ = new EasyContext(OPERATION_SEND, this);
	}

	int HandleMessage();
	int SendMessage();
	int SendMessage(char* buffer, int len);

	int				socket_;
	sockaddr_in		addr_;
	EasyAcceptor*	acceptor_;
	epoll_event		ev_;
	EasyContext*	recv_context_;
	EasyContext*	send_context_;
};

#endif
