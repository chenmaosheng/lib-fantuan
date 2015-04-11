#ifndef _H_TIME_SCOPE
#define _H_TIME_SCOPE

#include "easy_time.h"
#include <cstdio>

struct TimeScope
{
	TimeScope()
	{
		start = GET_CURR_TIME;
	}

	~TimeScope()
	{
		printf("cost time: %lluus\n", (GET_CURR_TIME - start) / 1000);
	}

	uint64 start;
};

#endif
