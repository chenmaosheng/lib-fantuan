#ifndef _H_EASY_SESSION
#define _H_EASY_SESSION

#include "common.h"

struct EasyConnection;
struct EasyPacket;
class EasyBaseServer;
class EasySession
{
public:
	EasySession();
	virtual ~EasySession();
	void	Reset();
	inline uint32	GetSessionID() const
	{
		return m_iSessionID;
	}

	inline void		SetSessionID(uint32 iSessionID)
	{
		m_iSessionID = iSessionID;
	}

	virtual int32	OnConnection(ConnID);
	virtual void	OnDisconnect();
	virtual void	OnData(uint32 iLen, char* pData);
	virtual void	Disconnect();
	virtual	int32	SendData(uint16 iTypeId, uint16 iLen, const char* pData);

protected:
	virtual int32	_HandlePacket(EasyPacket*);

protected:
	uint32			m_iSessionID;
	EasyConnection*	m_pConnection;
	uint16			m_iRecvBufLen;
	char			m_RecvBuf[MAX_INPUT_BUFFER];

public:
	static EasyBaseServer*	m_pServer;
};


#endif
