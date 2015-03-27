#include "easy_network.h"
#include "easy_worker.h"
#include "easy_acceptor.h"

int main(int argc, char* argv[])
{
	EasyNetwork::Init();
	EasyWorker* pWorker = EasyWorker::CreateWorker(2);
	EasyAcceptor* pAcceptor = EasyAcceptor::CreateAcceptor(0, 9001, pWorker);
	pAcceptor->Start();

	while (true)
	{
		Sleep(1000);
	}
}
