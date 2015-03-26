#include "easy_network.h"
#include "easy_worker.h"
#include "easy_acceptor.h"

int main(int argc, char* argv[])
{
	EasyNetwork::Init();
	EasyWorker* pWorker = EasyWorker::CreateWorker(2);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(9001);
	EasyAcceptor* pAcceptor = EasyAcceptor::CreateAcceptor(&addr, pWorker);
	pAcceptor->Start();

	while (true)
	{
		Sleep(1000);
	}
}
