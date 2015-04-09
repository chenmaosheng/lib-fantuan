#ifndef _H_EASY_BASECLIENT
#define _H_EASY_BASECLIENT

#include "common.h"
#include "dy_array.h"

struct SimpleHandler;
struct SimpleConnection;
struct EasyCommand;
class EasySession;
struct EasyPacket;
class SimpleWorker;
class EasyBaseClient
{
	// handle io event
	static bool __stdcall OnConnection(ConnID connId);
	static void __stdcall OnDisconnect(ConnID connId);
	static void __stdcall OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void __stdcall OnConnectFailed(void*);

public:
	EasyBaseClient();
	virtual ~EasyBaseClient();

	void			Login(const char* ip, uint16 port);
	void			Logout();
	void			PushCommand(EasyCommand*);
	virtual	int32	SendData(uint16 iTypeId, uint16 iLen, const char* pData);

private:
	virtual int32			_HandlePacket(EasyPacket*);
	virtual uint32			_Loop();
	static uint32			_CommandHandler(void*);
	virtual bool			_OnCommand(EasyCommand*);
	virtual bool			_OnCommandOnConnect(EasyCommand*);
	virtual bool			_OnCommandOnDisconnect(EasyCommand*);
	virtual bool			_OnCommandOnData(EasyCommand*);
	
private:
	SimpleHandler*			m_pHandler;
	SimpleConnection*		m_pConnection;
	SimpleWorker*			m_pWorker;
	DyArray<EasyCommand*>	m_CommandList;
	std::mutex				m_CommandMutex;
	std::condition_variable	m_CommandEvent;
	std::thread*			m_pCommandHandler;
	bool					m_bReadyForShutdown;
	uint16					m_iRecvBufLen;
	char					m_RecvBuf[MAX_INPUT_BUFFER];
};

#endif
