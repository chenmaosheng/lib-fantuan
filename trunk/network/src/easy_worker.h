#ifndef _H_EASY_WORKER
#define _H_EASY_WORKER

#include "common.h"

class EasyAcceptor;
class EasyWorker
{
public:
#ifdef WIN32
	// initialize worker with several threads
	void Init(uint32 iThreadCount);
	// destroy worker
	void Destroy();

	static EasyWorker* CreateWorker(uint32 iThreadCount);
	static void DestroyWorker(EasyWorker*);

	static uint32 WINAPI WorkerThread(PVOID);

public:
	HANDLE  iocp_;                  // binded iocp handle
	uint32  thread_count_;  // the number of threads
#endif

#ifdef _LINUX
	EasyWorker(EasyAcceptor* pAcceptor);
	static void* WorkerThread(void* ptr);

	pthread_t thread_;
#endif
};


#endif
