#include "easy_acceptor.h"
#include "easy_worker.h"
#include "easy_connection.h"
#include "easy_network.h"
#include "easy_contextpool.h"

bool __stdcall OnConnection(ConnID connId)
{
	printf("OnConnection::Get A new connection\n");
	return true;
}

void __stdcall OnDisconnect(ConnID connId)
{
	printf("OnDisconnect::Lost A connection\n");
}

void __stdcall OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	printf("OnData::len=%d\n", iLen);
}

void __stdcall OnConnectFailed(void*)
{
}

int main(int argc, char* argv[])
{
	EasyHandler handler;
	handler.OnConnection = OnConnection;
	handler.OnDisconnect = OnDisconnect;
	handler.OnData = OnData;
	handler.OnConnectFailed = OnConnectFailed;

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9001);
	addr.sin_addr.s_addr = htonl(0);

#ifdef WIN32
	EasyNetwork::Init();
	EasyWorker* pWorker = new EasyWorker(2);
	EasyContextPool* pContextPool = new EasyContextPool();
	EasyAcceptor* pAcceptor = new EasyAcceptor(&addr, pWorker, pContextPool, &handler);
	pAcceptor->Start();

	while (true)
	{
		Sleep(1000);
	}

	SAFE_DELETE(pWorker);
	SAFE_DELETE(pContextPool);
	SAFE_DELETE(pAcceptor);
#endif

#ifdef _LINUX
	EasyAcceptor* pAcceptor = new EasyAcceptor(&addr, &handler);
	EasyWorker* pWorker = new EasyWorker(pAcceptor);

	while (true)
	{
		if (pAcceptor->conn_)
		{
			char ch[65535] = {0};
			for (int i = 0; i < 65535; ++i)
			{
				ch[i] = 'a';
			}
			printf("ready to send message\n");
			pAcceptor->conn_->SendMessage(ch, strlen(ch));
		}
		sleep(1);	
	}


	delete pWorker;
	delete pAcceptor;

#endif

	return 0;
}