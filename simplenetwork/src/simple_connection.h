#ifndef _H_SIMPLE_CONNECTION
#define _H_SIMPLE_CONNECTION

#include "simple_handler.h"

struct SimpleContext;
struct SimpleConnection
{
	SimpleConnection();
	~SimpleConnection();

	int32	Connect(const char* ip, uint16 port, SimpleHandler* pHandler);
	int32	RecvData();
	int32	SendData(uint32 len, char* buf);
	void	Close();
	void	SetClient(void* pClient);
	void*	GetClient();

	SOCKET			socket_;
	SOCKADDR_IN		addr_;
	SimpleContext*	recv_context_;
	SimpleContext*	send_context_;
	SimpleHandler	handler_;
	void*			client_;
};
#endif
