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

struct EasyConnection;
class EasyContextPool;
struct EasyContext 
#ifdef WIN32
	: SLIST_ENTRY
{
	WSAOVERLAPPED		overlapped_;		// overlapped io
	WSABUF				wsabuf_;			// overlapped buf
	int32				operation_type_;	// overlapped operation type
	EasyConnection*		connection_;		// context's connection
	EasyContextPool*	context_pool_;		// the pool which is based on

	char				buffer_[(sizeof(SOCKADDR)+16)*2];		// received buffer, at least save 2 sockaddrs(acceptex needed)
#endif

#ifdef _LINUX
{
	char*				buffer_;
	int32				len_;
#endif
};

#endif
