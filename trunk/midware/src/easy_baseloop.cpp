#include "easy_baseloop.h"
#include "easy_command.h"
#include "easy_connection.h"
#include "easy_session.h"

EasyBaseLoop::EasyBaseLoop(uint32 iMaxSession) : m_iShutdownStatus(NOT_SHUTDOWN), m_iCurrTime(0), m_iDeltaTime(0), m_iWorldTime(0), m_iMaxSession(iMaxSession)
{
	m_SessionArray = new EasySession*[iMaxSession];
}

EasyBaseLoop::~EasyBaseLoop()
{
	SAFE_DELETE_ARRAY(m_SessionArray);
}

int32 EasyBaseLoop::Initialize()
{
	_InitializeSessionPool();
	
	m_iCurrTime = timeGetTime();
	m_pCommandHandler = new std::thread(&_CommandHandler, this);
	LOG_STT(_T("Initialize command handler"));
	return 0;
}

void EasyBaseLoop::Destroy()
{
	SAFE_DELETE(m_pCommandHandler);
}

void EasyBaseLoop::PushCommand(EasyCommand* pCommand)
{
	std::unique_lock<std::mutex> lock(m_CommandMutex);
	m_CommandEvent.notify_one();
	m_CommandList.push_back(pCommand);
}

int32 EasyBaseLoop::_InitializeSessionPool()
{
	for (uint32 i = 0; i < m_iMaxSession; ++i)
	{
		m_SessionArray[i] = m_SessionPool.Allocate();
		EASY_ASSERT(m_SessionArray[i] != NULL);
		m_SessionArray[i]->SetSessionID(i);
		m_SessionPool.Free(m_SessionArray[i]);
	}

	LOG_STT(_T("Initialize session pool"));

	return 0;
}

void EasyBaseLoop::_UpdateTimeControl()
{
	uint32 iLastTickTime = 0;
	iLastTickTime = m_iCurrTime;
	m_iCurrTime = timeGetTime();
	m_iDeltaTime = m_iCurrTime - iLastTickTime;
	m_iWorldTime = time(NULL);
}

uint32 EasyBaseLoop::_Loop()
{
	return 100;
}

EasySession* EasyBaseLoop::_GetSession(uint32 iSessionID)
{
	EASY_ASSERT(iSessionID <= m_iMaxSession);
	if (iSessionID >= m_iMaxSession)
	{
		LOG_ERR(_T("SessionId is invalid, sid=%08x"), iSessionID);
		return NULL;
	}
	return m_SessionArray[iSessionID];
}

bool EasyBaseLoop::_OnCommand(EasyCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONCONNECT:
		if (m_iShutdownStatus == NOT_SHUTDOWN)
		{
			return _OnCommandOnConnect(pCommand);
		}
		break;
	
	case COMMAND_ONDISCONNECT:
		return _OnCommandOnDisconnect(pCommand);

	case COMMAND_DISCONNECT:
		return _OnCommandDisconnect(pCommand);

	case COMMAND_ONDATA:
		if (m_iShutdownStatus == NOT_SHUTDOWN)
		{
			return _OnCommandOnData(pCommand);
		}
		break;

	case COMMAND_SENDDATA:
		if (m_iShutdownStatus == NOT_SHUTDOWN)
		{
			return _OnCommandSendData(pCommand);
		}
		break;

	case COMMAND_SHUTDOWN:
		_OnCommandShutdown();
		break;

	default:
		LOG_ERR(_T("Not implemented cmdid=%d"), pCommand->m_iCmdId);
		return false;
	}

	return true;
}

bool EasyBaseLoop::_OnCommandOnConnect(EasyCommand* pCommand)
{
	EasyCommandOnConnect* pCommandOnConnect = (EasyCommandOnConnect*)pCommand;
	EasySession* pSession = m_SessionPool.Allocate();
	m_SessionMap.insert(std::make_pair(pSession->GetSessionID(), pSession));
	pSession->OnConnection(pCommandOnConnect->m_ConnId);

	return true;
}

bool EasyBaseLoop::_OnCommandOnDisconnect(EasyCommand* pCommand)
{
	EasyCommandOnDisconnect* pCommandOnDisconnect = (EasyCommandOnDisconnect*)pCommand;
	EasyConnection* pConnection = (EasyConnection*)pCommandOnDisconnect->m_ConnId;
	EasySession* pSession = (EasySession*)pConnection->GetClient();
	if (pSession)
	{
		std::unordered_map<uint32, EasySession*>::iterator mit = m_SessionMap.find(pSession->GetSessionID());
		if (mit != m_SessionMap.end())
		{
			m_SessionMap.erase(mit);
		}
		pSession->OnDisconnect();
		m_SessionPool.Free(pSession);

		return true;
	}

	return false;
}

bool EasyBaseLoop::_OnCommandOnData(EasyCommand* pCommand)
{
	EasyCommandOnData* pCommandOnData = (EasyCommandOnData*)pCommand;
	EasyConnection* pConnection = (EasyConnection*)pCommandOnData->m_ConnId;
	EasySession* pSession = (EasySession*)pConnection->GetClient();
	if (pSession)
	{
		pSession->OnData(pCommandOnData->m_iLen, pCommandOnData->m_pData);
		return true;
	}
	return false;
}

bool EasyBaseLoop::_OnCommandDisconnect(EasyCommand* pCommand)
{
	EasyCommandDisconnect* pCommandDisconnect = (EasyCommandDisconnect*)pCommand;
	EasySession* pSession = m_SessionArray[pCommandDisconnect->m_iSessionId];
	if (pSession)
	{
		pSession->Disconnect();
		return true;
	}

	return false;
}

bool EasyBaseLoop::_OnCommandSendData(EasyCommand* pCommand)
{
	EasyCommandSendData* pCommandSendData = (EasyCommandSendData*)pCommand;
	EasySession* pSession = m_SessionArray[pCommandSendData->m_iSessionId];
	if (pSession)
	{
		if (pSession->SendData(pCommandSendData->m_iTypeId, pCommandSendData->m_iLen, pCommandSendData->m_pData) != 0)
		{
			LOG_ERR(_T("SendData failed, sid=%08x"), pCommandSendData->m_iSessionId);
			return false;
		}

		return true;
	}
	
	return false;
}

bool EasyBaseLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = READY_FOR_SHUTDOWN;
	for (std::unordered_map<uint32, EasySession*>::iterator mit = m_SessionMap.begin(); mit != m_SessionMap.end(); ++mit)
	{
		mit->second->Disconnect();
	}
	return true;
}

uint32 EasyBaseLoop::_CommandHandler(void* pParam)
{
	EasyBaseLoop* pBaseLoop = (EasyBaseLoop*)pParam;
	uint32 iSleepTime = 0;
	for (;;)
	{
		// update time control
		pBaseLoop->_UpdateTimeControl();
		std::unique_lock<std::mutex> lock(pBaseLoop->m_CommandMutex);
		if (pBaseLoop->m_CommandEvent.wait_for(lock, std::chrono::milliseconds(iSleepTime)) != std::cv_status::timeout)
		{
			for (DyArray<EasyCommand*>::iterator it = pBaseLoop->m_CommandList.begin(); it != pBaseLoop->m_CommandList.end(); ++it)
			{
				pBaseLoop->_OnCommand(*it);
				SAFE_DELETE(*it);
			}
			pBaseLoop->m_CommandList.clear();
		}
		lock.unlock();
		iSleepTime = pBaseLoop->_Loop();
	}

	return 0;
}