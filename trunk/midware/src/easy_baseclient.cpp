#include "easy_baseclient.h"
#include "simple_connection.h"
#include "simple_network.h"
#include "easy_dump.h"
#include "easy_command.h"
#include "easy_dispatcher.h"
#include "simple_worker.h"

bool __stdcall EasyBaseClient::OnConnection(ConnID connId)
{
	EasyBaseClient* pClient = (EasyBaseClient*)((SimpleConnection*)connId)->GetClient();
	EasyCommandOnConnect* pCommand = NULL;

	if (pClient->m_bReadyForShutdown)
	{
		return false;
	}

	pCommand = new EasyCommandOnConnect;
	pCommand->m_ConnId = connId;
	pClient->PushCommand(pCommand);

	return true;
}

void __stdcall EasyBaseClient::OnDisconnect(ConnID connId)
{
	EasyBaseClient* pClient = (EasyBaseClient*)((SimpleConnection*)connId)->GetClient();
	EasyCommandOnDisconnect* pCommand = NULL;

	if (pClient->m_bReadyForShutdown)
	{
		return;
	}

	pCommand = new EasyCommandOnDisconnect;
	pCommand->m_ConnId = connId;
	pClient->PushCommand(pCommand);
}

void __stdcall EasyBaseClient::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	EasyBaseClient* pClient = (EasyBaseClient*)((SimpleConnection*)connId)->GetClient();
	EasyCommandOnData* pCommand = NULL;

	if (pClient->m_bReadyForShutdown)
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
	pClient->PushCommand(pCommand);
}

void __stdcall EasyBaseClient::OnConnectFailed(void*)
{
}

EasyBaseClient::EasyBaseClient() : m_pConnection(new SimpleConnection), m_pHandler(new SimpleHandler), m_pWorker(NULL), 
	m_pCommandHandler(NULL), m_bReadyForShutdown(false), m_iRecvBufLen(0)
{
	m_RecvBuf[0] = '\0';
	EasyDump::Init();
	SimpleNetwork::Init();
	EasyLog::Instance()->Init();

	m_pHandler->OnConnection = &OnConnection;
	m_pHandler->OnDisconnect = &OnDisconnect;
	m_pHandler->OnData = &OnData;
	m_pHandler->OnConnectFailed = &OnConnectFailed;

	m_pCommandHandler = new std::thread(&_CommandHandler, this);
	LOG_STT(_T("Initialize command handler"));
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
	int32 iRet = 0;
	m_pConnection->SetClient(this);
	iRet = m_pConnection->Connect(ip, port, m_pHandler);
	if (iRet == -1)
	{
		LOG_ERR(_T("login error"));
	}

	LOG_STT(_T("login successfully"));

	m_pWorker = new SimpleWorker(m_pConnection);
}

void EasyBaseClient::Logout()
{
	m_bReadyForShutdown = true;
	m_pConnection->Close();
}

void EasyBaseClient::PushCommand(EasyCommand* pCommand)
{
	std::unique_lock<std::mutex> lock(m_CommandMutex);
	m_CommandEvent.notify_one();
	m_CommandList.push_back(pCommand);
}

int32 EasyBaseClient::SendData(uint16 iTypeId, uint16 iLen, const char* pData)
{
	char buf[MAX_INPUT_BUFFER] = {0};
	EasyPacket* pPacket = (EasyPacket*)buf;
	pPacket->m_iLen = iLen;
	pPacket->m_iTypeId = iTypeId;

	memcpy(pPacket->m_Buf, pData, iLen);
	m_pConnection->SendData(pPacket->m_iLen + PACKET_HEAD, buf);

	return 0;
}

uint32 EasyBaseClient::_Loop()
{
	return 100;
}

bool EasyBaseClient::_OnCommand(EasyCommand* pCommand)
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

	case COMMAND_ONDATA:
		if (!m_bReadyForShutdown)
		{
			return _OnCommandOnData(pCommand);
		}
		break;

	default:
		LOG_ERR(_T("Not implemented cmdid=%d"), pCommand->m_iCmdId);
		return false;
	}

	return true;
}

bool EasyBaseClient::_OnCommandOnConnect(EasyCommand* pCommand)
{
	EasyCommandOnConnect* pCommandOnConnect = (EasyCommandOnConnect*)pCommand;

	return true;
}

bool EasyBaseClient::_OnCommandOnDisconnect(EasyCommand* pCommand)
{
	EasyCommandOnDisconnect* pCommandOnDisconnect = (EasyCommandOnDisconnect*)pCommand;
	

	return false;
}

bool EasyBaseClient::_OnCommandOnData(EasyCommand* pCommand)
{
	EasyCommandOnData* pCommandOnData = (EasyCommandOnData*)pCommand;
	uint16 iCopyLen = 0;
	int32 iRet = 0;
	uint16 iLen = pCommandOnData->m_iLen;
	char* pData = pCommandOnData->m_pData;

	do
	{
		// the incoming length is no more than the last of buffer
		if (m_iRecvBufLen + iLen <= sizeof(m_RecvBuf))
		{
			memcpy(m_RecvBuf + m_iRecvBufLen, pData, iLen);
			m_iRecvBufLen += iLen;
			pData += iLen;
			iLen = 0;
		}
		else
		{
			iCopyLen = m_iRecvBufLen + iLen - sizeof(m_RecvBuf);
			memcpy(m_RecvBuf + m_iRecvBufLen, pData, iCopyLen);
			pData += iCopyLen;
			iLen -= iCopyLen;
			m_iRecvBufLen += iCopyLen;
		}	// step1: received a raw buffer

		while (m_iRecvBufLen >= PACKET_HEAD)	// step2: check if buffer is larger than header
		{
			EasyPacket* pPacket = (EasyPacket*)m_RecvBuf;
			uint16 iFullLength = pPacket->m_iLen+PACKET_HEAD;
			if (m_iRecvBufLen >= iFullLength)	// step3: cut specific size from received buffer
			{
				iRet = _HandlePacket(pPacket);
				if (iRet != 0)
				{
					return false;
				}

				if (m_iRecvBufLen > iFullLength)
				{
					memmove(m_RecvBuf, m_RecvBuf + iFullLength, m_iRecvBufLen - iFullLength);
				}
				m_iRecvBufLen -= iFullLength;
			}
			else
			{
				break;
			}
		}
	}while (iLen);

	return true;
}

uint32 EasyBaseClient::_CommandHandler(void* pParam)
{
	EasyBaseClient* pBaseClient = (EasyBaseClient*)pParam;
	uint32 iSleepTime = 0;
	for (;;)
	{
		std::unique_lock<std::mutex> lock(pBaseClient->m_CommandMutex);
		if (pBaseClient->m_CommandEvent.wait_for(lock, std::chrono::milliseconds(iSleepTime)) != std::cv_status::timeout)
		{
			for (auto it = pBaseClient->m_CommandList.begin(); it != pBaseClient->m_CommandList.end(); ++it)
			{
				pBaseClient->_OnCommand(*it);
				SAFE_DELETE(*it);
			}
			pBaseClient->m_CommandList.clear();
		}
		lock.unlock();
		iSleepTime = pBaseClient->_Loop();
	}

	return 0;
}

int32 EasyBaseClient::_HandlePacket(EasyPacket* pPacket)
{
	EasyDispatcher::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}