#include "easy_baseclient.h"
#include "simple_connection.h"
#include "simple_network.h"
#include "easy_dump.h"

bool __stdcall EasyBaseClient::OnConnection(ConnID connId)
{
	

	return true;
}

void __stdcall EasyBaseClient::OnDisconnect(ConnID connId)
{
	
}

void __stdcall EasyBaseClient::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	
}

void __stdcall EasyBaseClient::OnConnectFailed(void*)
{
}

EasyBaseClient::EasyBaseClient() : m_pConnection(new SimpleConnection), m_pHandler(new SimpleHandler)
{
	EasyDump::Init();
	SimpleNetwork::Init();
	EasyLog::Instance()->Init();

	m_pHandler->OnConnection = OnConnection;
	m_pHandler->OnDisconnect = OnDisconnect;
	m_pHandler->OnData = OnData;
	m_pHandler->OnConnectFailed = OnConnectFailed;
}

EasyBaseClient::~EasyBaseClient()
{
	SAFE_DELETE(m_pHandler);
	SAFE_DELETE(m_pConnection);
	EasyLog::Instance()->Destroy();
	SimpleNetwork::Destroy();
}

void EasyBaseClient::Login(const char* ip, uint16 port)
{
	int32 iRet = m_pConnection->Connect(ip, port, m_pHandler);
	if (iRet == -1)
	{
		LOG_ERR(_T("login error"));
	}

	LOG_STT(_T("login successfully"));
}

void EasyBaseClient::Logout()
{
	m_pConnection->Close();
}