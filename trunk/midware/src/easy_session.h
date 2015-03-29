#ifndef _H_EASY_SESSION
#define _H_EASY_SESSION

#include "common.h"

#pragma pack(1)
// better way to generate sessionid
union SessionId
{
	struct  
	{
		uint8 sequence_;		// when it's been reused, add 1
		uint16 session_index_;	// session index in session server
		uint8 serverId_;		// session server's id
	}sValue_;

	uint32 iValue_;

	SessionId()
	{
		iValue_ = 0;
	}
};
#pragma pack()

struct EasyConnection;
struct EasyPacket;
class EasyBaseServer;
class EasySession
{
public:
	EasySession();
	virtual ~EasySession();
	void	Reset();

	virtual int32	OnConnection(ConnID);
	virtual void	OnDisconnect();
	virtual void	OnData(uint32 iLen, char* pData);
	virtual void	Disconnect();
	virtual	int32	SendData(uint16 iTypeId, uint16 iLen, char* pData);

protected:
	virtual int32	_HandlePacket(EasyPacket*);

protected:
	uint32			m_iSessionID;
	EasyConnection*	m_pConnection;
	uint16			m_iRecvBufLen;
	char			m_RecvBuf[MAX_INPUT_BUFFER];

	static EasyBaseServer*	m_pServer;
};


#endif
