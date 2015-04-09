#ifndef _H_EASY_DISPATCHER
#define _H_EASY_DISPATCHER

#include "easy_stream.h"

struct EasyPacket
{
	uint16 m_iTypeId;	// 1st byte: filterId; 2nd byte: funcType
	uint16 m_iLen;		// buffer length, not include typeid and length itself
	char m_Buf[1];		// the 1st byte of buffer
};

#define PACKET_HEAD sizeof(uint16) + sizeof(uint16)

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
