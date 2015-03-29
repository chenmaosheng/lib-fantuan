#ifndef _H_EASY_BASESERVER
#define _H_EASY_BASESERVER

#include "common.h"

class EasyWorker;
class EasyContextPool;
class EasyAcceptor;
struct EasyHandler;
class EasyBaseLoop;
class EasyBaseServer
{
	// handle io event
	static bool __stdcall OnConnection(ConnID connId);
	static void __stdcall OnDisconnect(ConnID connId);
	static void __stdcall OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void __stdcall OnConnectFailed(void*);

public:
	EasyBaseServer();
	virtual ~EasyBaseServer();

	int32	Initialize(uint32 ip, uint16 port);
	void	Destroy();
	void	Shutdown();
	char*	PopOutputBuffer();

private:
	int32	_InitializeLog();
	void	_DestroyLog();

	int32	_InitializeNetwork(uint32 ip, uint16 port);
	void	_DestroyNetwork();

	int32	_InitializeMainLoop();
	void	_DestroyMainLoop();

	virtual EasyBaseLoop* _CreateServerLoop() = 0;

private:
	EasyAcceptor*		m_pAcceptor;
	EasyHandler*		m_pHandler;
	EasyWorker*			m_pWorker;
	EasyContextPool*	m_pContextPool;
	EasyBaseLoop*		m_pMainLoop;
	bool				m_bReadyForShutdown;
};

#endif
