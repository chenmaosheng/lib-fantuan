#ifndef _H_EASY_HANDLER
#define _H_EASY_HANDLER

#include "common.h"

typedef bool (__stdcall* ON_CONNECTION)(ConnID);
typedef void (__stdcall* ON_DISCONNECT)(ConnID);
typedef void (__stdcall* ON_DATA)(ConnID, uint32, char*);
typedef void (__stdcall* ON_CONNECT_FAILED)(void*);

struct EasyHandler
{
	ON_CONNECTION OnConnection;
	ON_DISCONNECT OnDisconnect;
	ON_DATA OnData;
	ON_CONNECT_FAILED OnConnectFailed;
};

#endif