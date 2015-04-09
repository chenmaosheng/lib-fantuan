#include "easy_session.h"
#include "easy_connection.h"
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
#ifdef WIN32
		EasyConnection::Close(m_pConnection);
#else
		m_pConnection->Close();
		SAFE_DELETE(m_pConnection);
#endif
	}

	Reset();
}

void EasySession::OnData(uint32 iLen, char* pData)
{
	uint16 iCopyLen = 0;
	int32 iRet = 0;

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
					return;
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

int32 EasySession::SendData(uint16 iTypeId, uint16 iLen, const char* pData)
{
#ifdef WIN32
	char* buf = m_pServer->PopOutputBuffer();
#endif
#ifdef _LINUX
	char* buf = m_SendBuf;
#endif
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