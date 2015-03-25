#ifndef _H_EASY_WORKER
#define _H_EASY_WORKER

#include "common.h"

struct EasyWorker
{
	EasyWorker(EasyAcceptor* pAcceptor);
	static void* WorkerThread(void* ptr);

	pthread_t thread_;
};

#endif
