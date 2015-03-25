#include "easy_acceptor.h"
#include "easy_worker.h"

int main(int argc, char* argv[])
{
	EasyAcceptor* pAcceptor = new EasyAcceptor(0, 9001);
	EasyWorker* pWorker = new EasyWorker(pAcceptor);

	while (true)
	{
		sleep(10000);
	}

	delete pWorker;
	delete pAcceptor;

	return 0;
}