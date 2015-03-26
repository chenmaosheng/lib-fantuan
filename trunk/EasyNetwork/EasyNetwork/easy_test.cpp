#include "easy_acceptor.h"
#include "easy_worker.h"
#include "easy_connection.h"

int main(int argc, char* argv[])
{
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

	return 0;
}