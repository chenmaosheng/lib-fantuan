#include "easy_session.h"
#include "easy_connection.h"
#include "easy_packet.h"
#include "easy_dispatcher.h"
#include "easy_baseserver.h"

EasyBaseServer* EasySession::m_pServer = NULL;

EasySession::EasySession() : m_iSessionID(0)
{
	Reset();
}

EasySession::~EasySession()
{

}

void EasySession::Reset()
{
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
}

int32 EasySession::OnConnection(ConnID connId)
{
	int32 iRet = 0;

	// reset sequence
	m_pConnection = (EasyConnection*)connId;
	m_pConnection->SetClient(this);

#ifdef WIN32
	m_pConnection->SetRefMax(256);
#endif
	return 0;
}

void EasySession::OnDisconnect()
{
	if (m_pConnection)
	{
		EasyConnection::Close(m_pConnection);
	}

	Reset();
}

void EasySession::OnData(uint32 iLen, char* pData)
{

}

void EasySession::Disconnect()
{
	if (m_pConnection)
	{
#ifdef WIN32
		m_pConnection->AsyncDisconnect();
#endif
	}
	else
	{
		OnDisconnect();
	}
}

int32 EasySession::SendData(uint16 iTypeId, uint16 iLen, char* pData)
{
	char* buf = m_pServer->PopOutputBuffer();
	EASY_ASSERT(buf);
	if (!buf)
	{
		return -1;
	}

	EasyPacket* pPacket = (EasyPacket*)buf;
	pPacket->m_iLen = iLen;
	pPacket->m_iTypeId = iTypeId;

	memcpy(pPacket->m_Buf, pData, iLen);
#ifdef WIN32
	m_pConnection->AsyncSend(pPacket->m_iLen + PACKET_HEAD, buf);
#endif
#ifdef _LINUX
	m_pConnection->SendMessage(pPacket->m_iLen + PACKET_HEAD, buf);
#endif

	return 0;
}

int32 EasySession::_HandlePacket(EasyPacket* pPacket)
{
	EasyDispatcher::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}