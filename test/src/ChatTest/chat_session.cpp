#include "chat_session.h"
#include "easy_dispatcher.h"
#include "chat_rpc.h"

void ChatSession::OnChatReq(uint32 iLen, char* message)
{
	ChatAck(this, iLen, message);
}

int32 EasyDispatcher::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((ChatSession*)pClient)->SendData(iTypeId, iLen, pBuf);
	return 0;
}
