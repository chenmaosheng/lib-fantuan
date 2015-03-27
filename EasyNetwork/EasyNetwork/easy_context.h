#ifndef _H_EASY_CONTEXT
#define _H_EASY_CONTEXT

#include "common.h"

enum
{
	INVALID_OPERATION,
	OPERATION_ACCEPT,		// accept operation
	OPERATION_CONNECT,		// connect operation
	OPERATION_DISCONNECT,	// disconnect operation
	OPERATION_RECV,			// receive data operation
	OPERATION_SEND,			// send data operation
};

#define CTXOFFSET	offsetof(EasyContext, overlapped_)	// overlapped_ offset
#define BUFOFFSET	offsetof(EasyContext, buffer_)		// buffer_ offset

class EasyConnection;
class EasyContext
{
public:
#ifdef WIN32
	WSAOVERLAPPED		overlapped_;		// overlapped io
	WSABUF				wsabuf_;			// overlapped buf
	int32				operation_type_;	// overlapped operation type
	EasyConnection*		connection_;		// context's connection
	
	char				buffer_[(sizeof(SOCKADDR)+16)*2];		// received buffer, at least save 2 sockaddrs(acceptex needed)

	static EasyContext* CreateContext(int32 iOperationType)
	{
		EasyContext* pContext = (EasyContext*)_aligned_malloc(sizeof(EasyContext)+MAX_BUFFER, MEMORY_ALLOCATION_ALIGNMENT);
		ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
		pContext->operation_type_ = iOperationType;
		pContext->wsabuf_.buf = (char*)pContext->buffer_;
		return pContext;
	}
#endif

#ifdef _LINUX
	EasyContext()
	{
		buffer_ = NULL;
		len_ = 0;
	}

	char*			buffer_;
	int				len_;
#endif
};

#endif
