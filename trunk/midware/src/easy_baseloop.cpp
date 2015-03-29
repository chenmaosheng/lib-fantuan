#include "easy_baseloop.h"

EasyBaseLoop::EasyBaseLoop() : m_iShutdownStatus(NOT_SHUTDOWN), m_iCurrTime(0), m_iDeltaTime(0), m_iWorldTime(0)
{

}

EasyBaseLoop::~EasyBaseLoop()
{
	
}

int32 EasyBaseLoop::Initialize()
{
	m_iCurrTime = timeGetTime();

	m_pCommandHandler = new std::thread(&_CommandHandler, this);
	return 0;
}

void EasyBaseLoop::Destroy()
{
	SAFE_DELETE(m_pCommandHandler);
}

void EasyBaseLoop::PushCommand(EasyCommand* pCommand)
{
	std::unique_lock<std::mutex> lock(m_CommandMutex);
	m_CommandList.push_back(pCommand);
}

uint32 EasyBaseLoop::_CommandHandler(void* pParam)
{
	return 0;
}