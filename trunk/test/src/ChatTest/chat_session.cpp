#include "chat_session.h"
#include "easy_dispatcher.h"
#include "chat_rpc.h"
#include "chat_server.h"

void ChatSession::OnPingReq(uint32 iVersion)
{
	PingAck(this, iVersion);
}

void ChatSession::OnChatReq(uint32 iLen, char* message)
{
	for (auto mit = m_pServer->m_SessionMap.begin(); mit != m_pServer->m_SessionMap.end(); ++mit)
	{
		ChatAck(mit->second, iLen, message);
	}
	//ChatAck(this, iLen, message);
}

int32 EasyDispatcher::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((ChatSession*)pClient)->SendData(iTypeId, iLen, pBuf);
	return 0;
}
