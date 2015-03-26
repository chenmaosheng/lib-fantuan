#ifndef _H_EASY_CONTEXT
#define _H_EASY_CONTEXT

#include "common.h"

struct EasyContext
{
	EasyContext()
	{
		buffer_ = NULL;
		len_ = 0;
	}

	char*			buffer_;
	int				len_;
};

#endif
