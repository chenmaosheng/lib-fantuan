#ifndef _H_EASY_BASECLIENT
#define _H_EASY_BASECLIENT

#include "common.h"

struct SimpleHandler;
struct SimpleConnection;
class EasyBaseClient
{
	// handle io event
	static bool __stdcall OnConnection(ConnID connId);
	static void __stdcall OnDisconnect(ConnID connId);
	static void __stdcall OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void __stdcall OnConnectFailed(void*);

public:
	EasyBaseClient();
	virtual ~EasyBaseClient();

	void	Login(const char* ip, uint16 port);
	void	Logout();

private:
	SimpleHandler*		m_pHandler;
	SimpleConnection*	m_pConnection;
};

#endif
