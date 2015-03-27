#ifndef _H_EASY_CONNECTION
#define _H_EASY_CONNECTION

#include "common.h"

class EasyContext;
class EasyWorker;
class EasyAcceptor;
class EasyConnection
{
public:
#ifdef WIN32
	SOCKET                  socket_;        
	SOCKADDR_IN             sockaddr_;              // connetion's address
	EasyWorker*				worker_;
	EasyAcceptor*			acceptor_;              // related acceptor
	EasyContext*			context_;               // initial context
	    
	// asynchronous disconnect
	void AsyncDisconnect();
	// asynchronous send, need pop a context first
	void AsyncSend(EasyContext*);
	// asynchronous receive, need pop a context first
	void AsyncRecv(EasyContext*);
	void AsyncSend(uint32 len, char* buf);

	// static function to create and close
	static EasyConnection* Create(EasyWorker* pWorker, EasyAcceptor* pAcceptor);
	static void Close(EasyConnection*); // attention: don't call this function if disconnect not called
	static void Delete(EasyConnection*);
#endif

#ifdef _LINUX

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
#endif
};

#endif
