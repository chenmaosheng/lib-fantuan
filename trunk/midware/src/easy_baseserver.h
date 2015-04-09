#ifndef _H_EASY_BASESERVER
#define _H_EASY_BASESERVER

#include "common.h"
#include "dy_array.h"
#include <unordered_map>
#include "easy_pool.h"
#include "easy_session.h"

class EasyWorker;
class EasyContextPool;
class EasyAcceptor;
struct EasyHandler;
struct EasyCommand;
struct EasyCommandOnConnect;
class EasyBaseServer
{
	// handle io event
	static bool __stdcall OnConnection(ConnID connId);
	static void __stdcall OnDisconnect(ConnID connId);
	static void __stdcall OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void __stdcall OnConnectFailed(void*);

public:
	EasyBaseServer(uint32 iMaxSession=MAX_SESSION);
	virtual ~EasyBaseServer();

	int32	Initialize(uint32 ip, uint16 port);
	void	Destroy();
	void	Shutdown();
	void	PushCommand(EasyCommand*);
	bool	IsReadyForShutdown() const
	{
		return m_bReadyForShutdown;
	}

	uint32	GetCurrTime() const
	{
		return m_iCurrTime;
	}

	uint32	GetDeltaTime() const
	{
		return m_iDeltaTime;
	}

	uint64	GetWorldTime() const
	{
		return m_iWorldTime;
	}

	char*	PopOutputBuffer();

private:
	int32	_InitializeLog();
	void	_DestroyLog();

	int32	_InitializeNetwork(uint32 ip, uint16 port);
	void	_DestroyNetwork();

	int32					_InitializeSessionPool();
	void					_UpdateTimeControl();
	virtual uint32			_Loop();
	static uint32			_CommandHandler(void*);
	virtual EasySession*	_CreateSession() = 0;
	EasySession*			_GetSession(uint32 iSessionID);
	virtual bool			_OnCommand(EasyCommand*);
	virtual bool			_OnCommandOnConnect(EasyCommand*);
	virtual bool			_OnCommandOnDisconnect(EasyCommand*);
	virtual bool			_OnCommandOnData(EasyCommand*);
	virtual bool			_OnCommandDisconnect(EasyCommand*);
	virtual bool			_OnCommandSendData(EasyCommand*);
	virtual bool			_OnCommandShutdown();

private:
	EasyAcceptor*			m_pAcceptor;
	EasyHandler*			m_pHandler;
	EasyWorker*				m_pWorker;
	EasyContextPool*		m_pContextPool;
	bool					m_bReadyForShutdown;

	DyArray<EasyCommand*>	m_CommandList;
	std::mutex				m_CommandMutex;
	std::condition_variable	m_CommandEvent;
	std::thread*			m_pCommandHandler;
	uint32					m_iCurrTime;	// current time, from server start on (ms)
	uint32					m_iDeltaTime;	// unit frame time cost (ms)
	uint64					m_iWorldTime;	// world real time (s)

	EasyPool<EasySession>	m_SessionPool;
	EasySession**			m_SessionArray;
	uint32					m_iMaxSession;

public:
	std::unordered_map<uint32, EasySession*>	m_SessionMap;
};

#endif
