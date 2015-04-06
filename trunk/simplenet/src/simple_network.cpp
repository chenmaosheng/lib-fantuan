#include "simple_network.h"

int32 SimpleNetwork::Init()
{
	WSADATA wsd;
	// start WSA
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		LOG_ERR(_T("WSAStartup failed"));
		return -1;
	}

	LOG_STT(_T("Intialize network success"));
	return 0;
}

void SimpleNetwork::Destroy()
{
	WSACleanup();
}