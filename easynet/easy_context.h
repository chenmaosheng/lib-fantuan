#ifndef _H_EASY_CONTEXT
#define _H_EASY_CONTEXT

#include "easy_connection.h"

#define OPERATION_RECV 1
#define OPERATION_SEND 2

struct EasyContext
{
	EasyContext(int iOperationType, EasyConnection* pConnection)
	{
		buffer_ = NULL;
		len_ = 0;
		operation_type_ = iOperationType;
		connection_ = pConnection;
	}

	EasyConnection* connection_;
	char*			buffer_;
	int				len_;
	int				operation_type_;
};

#endif
