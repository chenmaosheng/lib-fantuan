#include "easy_acceptor.h"
#include "easy_worker.h"
#include "easy_connection.h"
#include "easy_network.h"
#include "easy_contextpool.h"

int main(int argc, char* argv[])
{
#ifdef WIN32
	EasyNetwork::Init();
	EasyWorker* pWorker = EasyWorker::CreateWorker(2);
	EasyContextPool* pContextPool = EasyContextPool::CreateContextPool(65535, 65535);
	EasyHandler handler;
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9001);
	addr.sin_addr.s_addr = htonl(0);
	EasyAcceptor* pAcceptor = EasyAcceptor::CreateAcceptor(&addr, pWorker, pContextPool, &handler);
	pAcceptor->Start();

	while (true)
	{
		Sleep(1000);
	}

	EasyWorker::DestroyWorker(pWorker);
	EasyAcceptor::DestroyAcceptor(pAcceptor);
#endif

#ifdef _LINUX
	EasyAcceptor* pAcceptor = new EasyAcceptor(0, 9001);
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