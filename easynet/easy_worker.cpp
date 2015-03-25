#include "easy_worker.h"
#include "easy_acceptor.h"
#include "easy_connection.h"
#include <pthread.h>
EasyWorker::EasyWorker(EasyAcceptor* pAcceptor)
{
	pthread_create(&thread_, NULL, &EasyWorker::WorkerThread, pAcceptor);
}

void* EasyWorker::WorkerThread(void* ptr)
{
	EasyAcceptor* pAcceptor = (EasyAcceptor*)ptr;
	do
	{
		int nfds = epoll_wait(pAcceptor->epfd_, pAcceptor->events_, 20, 500);
		for (int i = 0; i < nfds; ++i)
		{
			if (pAcceptor->events_[i].data.fd == pAcceptor->socket_)
			{
				pAcceptor->AcceptConnection();
			}
			else if (pAcceptor->events_[i].events & EPOLLIN)
			{
				EasyConnection* pConnection = (EasyConnection*)pAcceptor->events_[i].data.ptr;
				pConnection->HandleMessage();
			}
			else if (pAcceptor->events_[i].events & EPOLLOUT)
			{
				printf("trigger, epolllout\n");
				EasyConnection* pConnection = (EasyConnection*)pAcceptor->events_[i].data.ptr;
				pConnection->SendMessage();
			}
		}
	}while (true);
}
