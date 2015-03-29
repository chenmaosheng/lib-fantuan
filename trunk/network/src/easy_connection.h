#ifndef _H_EASY_CONNECTION
#define _H_EASY_CONNECTION

#include "easy_handler.h"

struct EasyContext;
class EasyContextPool;
class EasyWorker;
class EasyAcceptor;

#ifdef WIN32
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

	void				SetClient(void*);
	void*				GetClient();
	void				SetRefMax(uint16 iMax);
	bool				IsConnected();

	// static function to create and close
	static EasyConnection*	Create(EasyHandler* pHandler, EasyContextPool* pContextPool, EasyWorker* pWorker, EasyAcceptor* pAcceptor);
	static bool				Connect(PSOCKADDR_IN pAddr, EasyHandler* pHandler, EasyContextPool* pContextPool, EasyWorker* pWorker, void* pClient);
	static void				Close(EasyConnection*);	// attention: don't call this function if disconnect not called
	static void				Delete(EasyConnection*);
};

#endif

#ifdef _LINUX

struct EasyConnection
{
	EasyConnection(EasyAcceptor* pAcceptor);

	int HandleMessage();
	int SendMessage();
	int SendMessage(char* buffer, int len);
	void	SetClient(void*);
	void*	GetClient();

	static void				Close(EasyConnection*);

	int				socket_;
	sockaddr_in		addr_;
	EasyAcceptor*	acceptor_;
	epoll_event		ev_;
	EasyContext*	recv_context_;
	EasyContext*	send_context_;
	sem_t			sem_;
	void*			client_;
};
#endif

#endif
