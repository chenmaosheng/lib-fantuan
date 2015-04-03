#include "chat_rpc.h"
#include "easy_packet.h"
#include "easy_dispatcher.h"
#include "chat_session.h"

bool __stdcall PingReq_Callback(void* pClient, InputStream& stream)
{
	ChatSession* pSession = (ChatSession*)pClient;
	uint32 iVersion = 0;
	if (!stream.Serialize(iVersion)) return false;
	pSession->OnPingReq(iVersion);
	return true;
}

bool __stdcall ChatReq_Callback(void* pClient, InputStream& stream)
{
	ChatSession* pSession = (ChatSession*)pClient;
	char message[MAX_INPUT_BUFFER] = {0};
	uint32 len = 0;
	if (!stream.Serialize(len)) return false;
	if (!stream.Serialize(len, message)) return false;
	message[len] = '\0';
	pSession->OnChatReq(len, message);
	return true;
}

static EasyDispatcher::Func func[] = 
{
	PingReq_Callback,
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

int32 PingAck(void* pServer, uint32 iVersion)
{
	OutputStream stream;
	if (!stream.Serialize(iVersion)) return -1;
	EasyDispatcher::SendPacket(pServer, 0, stream.GetDataLength(), stream.GetBuffer());
	return 0;
}

int32 ChatAck(void* pServer, uint32 iLen, char* strMessage)
{
	OutputStream stream;
	if (!stream.Serialize(iLen)) return -1;
	if (!stream.Serialize(iLen, strMessage)) return -1;
	EasyDispatcher::SendPacket(pServer, 1, stream.GetDataLength(), stream.GetBuffer());
	return 0;
}
