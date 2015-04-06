#ifndef _H_SIMPLE_WORKER
#define _H_SIMPLE_WORKER

#include "common.h"

struct SimpleConnection;
class SimpleWorker
{
public:
	SimpleWorker(SimpleConnection*);
	~SimpleWorker();

	static uint32 WINAPI SimpleThread(void*);

	HANDLE thread_;
};
#endif
