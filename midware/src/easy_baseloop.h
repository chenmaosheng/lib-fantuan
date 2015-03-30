#ifndef _H_EASY_BASELOOP
#define _H_EASY_BASELOOP

#include "common.h"
#include "dy_array.h"
#include <unordered_map>
#include "easy_pool.h"
#include "easy_session.h"

struct EasyCommand;
struct EasyCommandOnConnect;
class EasyBaseLoop
{
public:
	enum { NOT_SHUTDOWN, READY_FOR_SHUTDOWN, };

	EasyBaseLoop(uint32 iMaxSession=MAX_SESSION);
	virtual ~EasyBaseLoop();

	int32	Initialize();
	void	Destroy();
	void	PushCommand(EasyCommand*);
	bool	IsReadyForShutdown() const
	{
		return m_iShutdownStatus == READY_FOR_SHUTDOWN;
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

protected:
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

protected:
	int32					m_iShutdownStatus;
	DyArray<EasyCommand*>	m_CommandList;
	std::mutex				m_CommandMutex;
	std::condition_variable	m_CommandEvent;
	std::thread*			m_pCommandHandler;
	uint32					m_iCurrTime;	// current time, from server start on (ms)
	uint32					m_iDeltaTime;	// unit frame time cost (ms)
	uint64					m_iWorldTime;	// world real time (s)

	std::unordered_map<uint32, EasySession*>	m_SessionMap;
	EasyPool<EasySession>	m_SessionPool;
	EasySession**			m_SessionArray;
	uint32					m_iMaxSession;
};


#endif
