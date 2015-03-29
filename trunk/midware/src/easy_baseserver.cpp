#include "easy_baseserver.h"
#include "easy_network.h"
#include "easy_worker.h"
#include "easy_contextpool.h"
#include "easy_acceptor.h"
#include "easy_handler.h"
#include "easy_baseloop.h"

bool __stdcall EasyBaseServer::OnConnection(ConnID connId)
{
	
	return true;
}

void __stdcall EasyBaseServer::OnDisconnect(ConnID connId)
{
	
}

void __stdcall EasyBaseServer::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	
}

void __stdcall EasyBaseServer::OnConnectFailed(void*)
{
}

EasyBaseServer::EasyBaseServer() 
	: m_pAcceptor(NULL), m_pHandler(NULL), m_pWorker(NULL), m_pContextPool(NULL), m_pMainLoop(NULL), m_bReadyForShutdown(false)
{

}

EasyBaseServer::~EasyBaseServer()
{

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

	iRet = _InitializeMainLoop();
	if (iRet != 0)
	{
		LOG_ERR(_T("Initialize MainLoop failed"));
		return -3;
	}

	LOG_STT(_T("Initialize success, server is started!"));

	return 0;
}

void EasyBaseServer::Destroy()
{
	LOG_STT(_T("Start to destroy server"));

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

int32 EasyBaseServer::_InitializeMainLoop()
{
	m_pMainLoop = _CreateServerLoop();
	m_pMainLoop->Initialize();

	return 0;
}

void EasyBaseServer::_DestroyMainLoop()
{
	m_pMainLoop->Destroy();
	SAFE_DELETE(m_pMainLoop);
}