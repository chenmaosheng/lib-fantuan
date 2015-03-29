#ifndef _H_EASY_COMMAND
#define _H_EASY_COMMAND

#include "common.h"

enum
{
	COMMAND_ONCONNECT,		// receive connection
	COMMAND_ONDISCONNECT,	// receive disconnect
	COMMAND_ONDATA,			// receive data
	COMMAND_SENDDATA,		// send data
	COMMAND_SHUTDOWN,		// ready to shutdown
	COMMAND_DISCONNECT,		// force disconnect
	COMMAND_ONSESSIONDISCONNECT, // receive session disconnect

	COMMAND_APPSTART = 1000,
};

struct EasyCommand 
{
	int32 m_iCmdId;
};

template<uint16 iCmdId>
struct EasyCommandT : public EasyCommand
{
	const static uint16 CmdId = iCmdId;

	EasyCommandT()
	{
		m_iCmdId = CmdId;
	}
};

struct EasyCommandOnConnect : public EasyCommandT<COMMAND_ONCONNECT>
{
	EasyCommandOnConnect()
	{
		m_ConnId = NULL;
	}

	ConnID m_ConnId;
};

struct EasyCommandOnDisconnect : public EasyCommandT<COMMAND_ONDISCONNECT>
{
	EasyCommandOnDisconnect()
	{
		m_ConnId = NULL;
	}

	ConnID m_ConnId;
};

struct EasyCommandOnData : public EasyCommandT<COMMAND_ONDATA>
{
	EasyCommandOnData();
	~EasyCommandOnData();
	bool CopyData(uint16 iLen, const char* pData);

	ConnID m_ConnId;
	uint16 m_iLen;
	char* m_pData;
};

struct EasyCommandSendData : public EasyCommandT<COMMAND_SENDDATA>
{
	EasyCommandSendData();
	~EasyCommandSendData();
	bool CopyData(uint16 iLen, const char* pData);

	uint32 m_iSessionId;
	uint16 m_iTypeId;
	uint16 m_iLen;
	char* m_pData;
};

struct EasyCommandShutdown : public EasyCommandT<COMMAND_SHUTDOWN>
{
};

struct EasyCommandDisconnect : public EasyCommandT<COMMAND_DISCONNECT>
{
	EasyCommandDisconnect()
	{
		m_iSessionId = 0;
		m_iReason = 0;
	}

	uint32 m_iSessionId;
	uint8 m_iReason;
};

struct EasyCommandOnSessionDisconnect : public EasyCommandT<COMMAND_ONSESSIONDISCONNECT>
{
	EasyCommandOnSessionDisconnect()
	{
		m_iSessionId = 0;
	}

	uint32 m_iSessionId;
};

#endif
