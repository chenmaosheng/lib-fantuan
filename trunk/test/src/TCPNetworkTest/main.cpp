#include "common.h"
#include <windows.h>

typedef void (*InitNetwork)();
typedef void (*DestroyNetwork)();
typedef void* (*CreateAcceptor)(uint32 ip, uint16 port, uint32 count, bool (__stdcall*)(ConnID), void (__stdcall*)(ConnID), void (__stdcall*)(ConnID, uint32, char*), void (__stdcall*)(void*));
typedef void (*StartAcceptor)(void*);

bool __stdcall OnConnection(ConnID)
{
	return true;
}

void __stdcall OnDisconnect(ConnID)
{

}

void __stdcall OnData(ConnID, uint32, char*)
{

}

void __stdcall OnConnectFailed(void*)
{

}

int main(int argc, char* argv[])
{
	HMODULE dll = LoadLibrary(L"TCPNetwork.dll");
	if (dll == NULL)
	{
		return -1;
	}
	InitNetwork initNetwork = InitNetwork(GetProcAddress(dll, "InitEasyTCP"));
	DestroyNetwork destroyNetwork = DestroyNetwork(GetProcAddress(dll, "DestroyEasyTCP"));
	CreateAcceptor createAcceptor = CreateAcceptor(GetProcAddress(dll, "CreateAcceptor"));
	StartAcceptor startAcceptor = StartAcceptor(GetProcAddress(dll, "StartAcceptor"));

	initNetwork();
	void* acceptor = createAcceptor(0, 9001, 2, OnConnection, OnDisconnect, OnData, OnConnectFailed);
	startAcceptor(acceptor);

	while (true)
	{
		easy_sleep(100);
	}
	destroyNetwork();

	return 0;
}