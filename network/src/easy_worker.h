#ifndef _H_EASY_WORKER
#define _H_EASY_WORKER

#include "common.h"

class EasyAcceptor;
class EasyWorker
{
public:
#ifdef WIN32
	EasyWorker(uint32 iThreadCount=2);
	~EasyWorker();

	static uint32 WorkerThread(void*);

public:
	HANDLE  iocp_;			// binded iocp handle
	uint32  thread_count_;	// the number of threads
#endif

#ifdef _LINUX
	EasyWorker(EasyAcceptor* pAcceptor);
	static uint32 WorkerThread(void*);
#endif
};


#endif
