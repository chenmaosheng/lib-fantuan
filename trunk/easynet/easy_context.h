#ifndef _H_EASY_CONTEXT
#define _H_EASY_CONTEXT

#include "easy_connection.h"

enum
{
	INVALID_OPERATION,
	OPERATION_RECV,			// receive data operation
	OPERATION_SEND,			// send data operation
};

struct EasyContext
{
	EasyContext(int iOperationType, EasyConnection* pConnection)
	{
		buffer_[0] = '\0';
		len_ = 0;
		operation_type_ = iOperationType;
		connection_ = pConnection;
	}

	EasyConnection* connection_;
	char			buffer_[1024];
	int				len_;
	int				operation_type_;
};
