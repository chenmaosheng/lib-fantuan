#include "chat_client.h"
#include "chat_rpc.h"

int32 EasyDispatcher::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	((ChatClient*)pClient)->SendData(iTypeId, iLen, pBuf);
	return 0;
}

ChatClient* g_pClient = NULL;

void SendCommand()
{
	char command[128] = {0};
	char message[128] = {0};
	scanf_s("%s", &command, _countof(command));
	if (_stricmp(command, "Ping") == 0)
	{
		ClientSend::PingReq(g_pClient, 0);
	}
	else if (_stricmp(command, "Chat") == 0)
	{
		scanf_s("%s", &message, _countof(message));
		ClientSend::ChatReq(g_pClient, strlen(message), message);
	}
}

int __cdecl _tmain(int argc, char **argv)
{
	g_pClient = new ChatClient;
	g_pClient->Login("127.0.0.1", 9001);

	while (true)
	{
		SendCommand();
	}

	return 0;
}
