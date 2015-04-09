#include "easy_baseserver.h"
#include "easy_command.h"
#include "easy_network.h"
#include "easy_connection.h"
#include "easy_acceptor.h"
#include "easy_worker.h"
#include "easy_contextpool.h"
#include "easy_handler.h"
#include "easy_session.h"

#ifdef _USE_ALLCATOR
#include "allocator.h"
void* operator new(size_t n)
{
	return FT_Alloc::allocate(n);
}

void* operator new[](size_t n)
{
	return operator new(n);
}

void operator delete(void* ptr)
{
	FT_Alloc::deallocate(ptr);
}

void operator delete[](void* ptr)
{
	operator delete(ptr);
}
#endif


bool __stdcall EasyBaseServer::OnConnection(ConnID connId)
{
	EasyBaseServer* pServer = (EasyBaseServer*)((EasyConnection*)connId)->acceptor_->GetServer();
	EasyCommandOnConnect* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return false;
	}

	pCommand = new EasyCommandOnConnect;
	pCommand->m_ConnId = connId;
	pServer->PushCommand(pCommand);

	return true;
}

void __stdcall EasyBaseServer::OnDisconnect(ConnID connId)
{
	EasyBaseServer* pServer = (EasyBaseServer*)((EasyConnection*)connId)->acceptor_->GetServer();
	EasyCommandOnDisconnect* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return;
	}

	pCommand = new EasyCommandOnDisconnect;
	pCommand->m_ConnId = connId;
	pServer->PushCommand(pCommand);
}

void __stdcall EasyBaseServer::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	EasyBaseServer* pServer = (EasyBaseServer*)((EasyConnection*)connId)->acceptor_->GetServer();
	EasyCommandOnData* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return;
	}

	pCommand = new EasyCommandOnData;
	pCommand->m_ConnId = connId;
	if (!pCommand->CopyData(iLen, pBuf))
	{
		SAFE_DELETE(pCommand);
		return;
	}
	pServer->PushCommand(pCommand);
}

void __stdcall EasyBaseServer::OnConnectFailed(void*)
{
}

EasyBaseServer::EasyBaseServer(uint32 iMaxSession) 
	: m_pAcceptor(NULL), m_pHandler(NULL), m_pWorker(NULL), m_pContextPool(NULL), m_bReadyForShutdown(false),
	m_iCurrTime(0), m_iDeltaTime(0), m_iWorldTime(0), m_iMaxSession(iMaxSession)
{
	m_SessionArray = new EasySession*[iMaxSession];
}

EasyBaseServer::~EasyBaseServer()
{
	SAFE_DELETE_ARRAY(m_SessionArray);
}

int32 EasyBaseServer::Initialize(uint32 ip, uint16 port)
{
	int32 iRet = 0;
	iRet = _InitializeLog();
	if (iRet != 0)
	{
		EASY_ASSERT(false &&_T("Init log error"));
		return -1;
	}

	iRet = _InitializeNetwork(ip, port);
	if (iRet != 0)
	{
		LOG_ERR(_T("Initialize Network failed"));
		return -2;
	}

	_InitializeSessionPool();
	_UpdateTimeControl();

	m_pCommandHandler = new std::thread(&_CommandHandler, this);
	LOG_STT(_T("Initialize command handler"));

	EasySession::m_pServer = this;

	LOG_STT(_T("Initialize success, server is started!"));

	return 0;
}

void EasyBaseServer::Destroy()
{
	LOG_STT(_T("Start to destroy server"));

	SAFE_DELETE(m_pCommandHandler);
	_DestroyNetwork();
	_DestroyLog();
}

void EasyBaseServer::Shutdown()
{

}

char* EasyBaseServer::PopOutputBuffer()
{
#ifdef WIN32
	if (m_pContextPool)
	{
		return m_pContextPool->PopOutputBuffer();
	}
#else
	EASY_ASSERT(false && _T("Not Implemented"));
#endif
	
	return NULL;
}

void EasyBaseServer::PushCommand(EasyCommand* pCommand)
{
	std::unique_lock<std::mutex> lock(m_CommandMutex);
	m_CommandEvent.notify_one();
	m_CommandList.push_back(pCommand);
}

int32 EasyBaseServer::_InitializeLog()
{
	EasyLog::Instance()->Init();

	LOG_STT(_T("Initialize log system success"));

	return 0;
}

void EasyBaseServer::_DestroyLog()
{
	EasyLog::Instance()->Destroy();
}

int32 EasyBaseServer::_InitializeNetwork(uint32 ip, uint16 port)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);

	m_pContextPool = new EasyContextPool;
	m_pHandler = new EasyHandler;
	m_pHandler->OnConnection = &OnConnection;
	m_pHandler->OnDisconnect = &OnDisconnect;
	m_pHandler->OnData = &OnData;
	m_pHandler->OnConnectFailed = &OnConnectFailed;

#ifdef WIN32
	int32 iRet = 0;
	iRet = EasyNetwork::Init();
	EASY_ASSERT(iRet == 0);
	if (iRet != 0)
	{
		LOG_ERR(_T("Initialize Winsock failed"));
		return -4;
	}

	LOG_STT(_T("Initialize Winsock success"));

	m_pWorker = new EasyWorker;
	// create acceptor to receive connection
	m_pAcceptor = new EasyAcceptor(&addr, m_pWorker, m_pContextPool, m_pHandler);
	m_pAcceptor->Start();
#endif

#ifdef _LINUX
	m_pAcceptor = new EasyAcceptor(&addr, m_pHandler);
	m_pWorker = new EasyWorker(m_pAcceptor);
#endif
	m_pAcceptor->SetServer(this);

	LOG_STT(_T("Initialize network success, IP=%d, port=%d"), ip, port);

	return 0;
}

void EasyBaseServer::_DestroyNetwork()
{
#ifdef WIN32
	SAFE_DELETE(m_pAcceptor);
	SAFE_DELETE(m_pWorker);
#endif

#ifdef _LINUX
	SAFE_DELETE(m_pWorker);
	SAFE_DELETE(m_pAcceptor);
#endif

	SAFE_DELETE(m_pHandler);
	SAFE_DELETE(m_pContextPool);
}

int32 EasyBaseServer::_InitializeSessionPool()
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

void EasyBaseServer::_UpdateTimeControl()
{
	uint32 iLastTickTime = 0;
	iLastTickTime = m_iCurrTime;
	m_iCurrTime = timeGetTime();
	m_iDeltaTime = m_iCurrTime - iLastTickTime;
	m_iWorldTime = time(NULL);
}

uint32 EasyBaseServer::_Loop()
{
	return 100;
}

EasySession* EasyBaseServer::_GetSession(uint32 iSessionID)
{
	EASY_ASSERT(iSessionID <= m_iMaxSession);
	if (iSessionID >= m_iMaxSession)
	{
		LOG_ERR(_T("SessionId is invalid, sid=%08x"), iSessionID);
		return NULL;
	}
	return m_SessionArray[iSessionID];
}

bool EasyBaseServer::_OnCommand(EasyCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONCONNECT:
		if (!m_bReadyForShutdown)
		{
			return _OnCommandOnConnect(pCommand);
		}
		break;

	case COMMAND_ONDISCONNECT:
		return _OnCommandOnDisconnect(pCommand);

	case COMMAND_DISCONNECT:
		return _OnCommandDisconnect(pCommand);

	case COMMAND_ONDATA:
		if (!m_bReadyForShutdown)
		{
			return _OnCommandOnData(pCommand);
		}
		break;

	case COMMAND_SENDDATA:
		if (!m_bReadyForShutdown)
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

bool EasyBaseServer::_OnCommandOnConnect(EasyCommand* pCommand)
{
	EasyCommandOnConnect* pCommandOnConnect = (EasyCommandOnConnect*)pCommand;
	EasySession* pSession = m_SessionPool.Allocate();
	m_SessionMap.insert(std::make_pair(pSession->GetSessionID(), pSession));
	pSession->OnConnection(pCommandOnConnect->m_ConnId);

	return true;
}

bool EasyBaseServer::_OnCommandOnDisconnect(EasyCommand* pCommand)
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

bool EasyBaseServer::_OnCommandOnData(EasyCommand* pCommand)
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

bool EasyBaseServer::_OnCommandDisconnect(EasyCommand* pCommand)
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

bool EasyBaseServer::_OnCommandSendData(EasyCommand* pCommand)
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

bool EasyBaseServer::_OnCommandShutdown()
{
	m_bReadyForShutdown = true;
	for (std::unordered_map<uint32, EasySession*>::iterator mit = m_SessionMap.begin(); mit != m_SessionMap.end(); ++mit)
	{
		mit->second->Disconnect();
	}
	return true;
}

uint32 EasyBaseServer::_CommandHandler(void* pParam)
{
	EasyBaseServer* pBaseServer = (EasyBaseServer*)pParam;
	uint32 iSleepTime = 0;
	for (;;)
	{
		// update time control
		pBaseServer->_UpdateTimeControl();
		std::unique_lock<std::mutex> lock(pBaseServer->m_CommandMutex);
		if (pBaseServer->m_CommandEvent.wait_for(lock, std::chrono::milliseconds(iSleepTime)) != std::cv_status::timeout)
		{
			for (auto it = pBaseServer->m_CommandList.begin(); it != pBaseServer->m_CommandList.end(); ++it)
			{
				pBaseServer->_OnCommand(*it);
				SAFE_DELETE(*it);
			}
			pBaseServer->m_CommandList.clear();
		}
		lock.unlock();
		iSleepTime = pBaseServer->_Loop();
	}

	return 0;
}