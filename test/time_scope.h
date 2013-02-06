#ifndef _H_TIME_SCOPE
#define _H_TIME_SCOPE

#include "accurate_time.h"
#include <cstdio>

struct TimeScope
{
	TimeScope()
	{
		start = GET_TIME();
	}

	~TimeScope()
	{
		printf("cost time: %lluus\n", (GET_TIME() - start) / 1000);
	}

	uint64 start;
};

#endif
