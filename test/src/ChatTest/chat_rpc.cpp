#include "chat_rpc.h"
#include "easy_packet.h"
#include "easy_dispatcher.h"
#include "chat_session.h"

bool CALLBACK ChatReq_Callback(void* pClient, Stream& stream)
{
	ChatSession* pSession = (ChatSession*)pClient;
	char message[MAX_INPUT_BUFFER] = {0};
	uint32 len = 0;
	if (!stream.DeSerialize(len)) return false;
	if (!stream.DeSerialize(len, message)) return false;
	message[len] = '\0';
	pSession->OnChatReq(len, message);
	return true;
}

static EasyDispatcher::Func func[] = 
{
	ChatReq_Callback,
};

struct ClientDispatcher
{
	ClientDispatcher()
	{
		EasyDispatcher::m_pFuncList = func;
	}
};

static ClientDispatcher clientDispatcher;

int32 ChatAck(void* pServer, uint32 iLen, char* strMessage)
{
	Stream stream;
	if (!stream.Serialize(iLen)) return -1;
	if (!stream.Serialize(iLen, strMessage)) return -1;
	EasyDispatcher::SendPacket(pServer, 1, stream.GetDataLength(), stream.GetBuffer());
	return 0;
}
