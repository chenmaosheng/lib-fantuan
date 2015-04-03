#ifndef _H_EASY_DISPATCHER
#define _H_EASY_DISPATCHER

#include "easy_stream.h"

class EasyDispatcher
{
public:
	// receive data from client
	static bool OnPacketReceived(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);
	static int32 SendPacket(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);

public:
	typedef bool (__stdcall* Func)(void*, InputStream&);
	static Func* m_pFuncList;
};

#endif
