#ifndef _H_EASY_BASELOOP
#define _H_EASY_BASELOOP

#include "common.h"
#include <vector>

struct EasyCommand;
class EasyBaseLoop
{
public:
	enum { NOT_SHUTDOWN, START_SHUTDOWN, READY_FOR_SHUTDOWN, };

	EasyBaseLoop();
	virtual ~EasyBaseLoop();

	int32	Initialize();
	void	Destroy();

	void	PushCommand(EasyCommand*);

	uint32	GetCurrTime() const;
	uint32	GetDeltaTime() const;
	uint64	GetWorldTime() const;

protected:
	static uint32 _CommandHandler(void*);

protected:
	int32					m_iShutdownStatus;
	std::vector<EasyCommand*>	m_CommandList;
	std::mutex				m_CommandMutex;
	std::thread*			m_pCommandHandler;
	uint32					m_iCurrTime;	// current time, from server start on (ms)
	uint32					m_iDeltaTime; // unit frame time cost (ms)
	uint64					m_iWorldTime; // world real time (s)
};


#endif
