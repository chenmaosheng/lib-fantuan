#ifndef _H_CHAT_RPC
#define _H_CHAT_RPC

#include "common.h"
#include "easy_dispatcher.h"
#include "easy_log.h"
#include "util.h"

class ClientSend
{
public:
	static int32 PingReq(void* pServer, uint32 iVersion)
	{
		OutputStream stream;
		if (!stream.Serialize(iVersion)) return -1;
		EasyDispatcher::SendPacket(pServer, 0, stream.GetDataLength(), stream.GetBuffer());
		return 0;
	}

	static int32 ChatReq(void* pServer, uint32 iLen, char* message)
	{
		OutputStream stream;
		if (!stream.Serialize(iLen)) return -1;
		if (!stream.Serialize(iLen, message)) return -1;
		EasyDispatcher::SendPacket(pServer, 1, stream.GetDataLength(), stream.GetBuffer());
		return 0;
	}
};

class ClientRecv
{
public:
	static void PingAck(void* pClient, uint32 iVersion)
	{
	}

	static bool CALLBACK PingAck_Callback(void* pClient, InputStream& stream)
	{
		uint32 iVersion;
		if (!stream.Serialize(iVersion)) return false;
		ClientRecv::PingAck(pClient, iVersion);
		return true;
	}

	static void ChatAck(void* pClient, uint32 len, char* message)
	{
		TCHAR szMessage[128] = {0};
		MultiByteToWideChar(CP_UTF8, 0, message, 128, szMessage, 128);
		LOG_DBG(_T("len=%d, message=%s"), len, szMessage);
	}

	static bool CALLBACK ChatAck_Callback(void* pClient, InputStream& stream)
	{
		char message[1024] = {0};
		uint32 len = 0;
		if (!stream.Serialize(len)) return false;
		if (!stream.Serialize(len, message)) return false;
		ClientRecv::ChatAck(pClient, len, message);
		return true;
	}
};

static EasyDispatcher::Func func[] = 
{
	ClientRecv::PingAck_Callback,
	ClientRecv::ChatAck_Callback,
	NULL
};

struct ClientDispatcher
{
	ClientDispatcher()
	{
		EasyDispatcher::m_pFuncList = func;
	}
};

static ClientDispatcher clientDispatcher;

#endif
