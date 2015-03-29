#ifndef _H_COMMON
#define _H_COMMON

#include "base.h"
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <stdexcept>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include "easy_log.h"

#define MAXLINE					1024
#define MAX_INPUT_BUFFER		1024	// max size from client to server
#define MAX_OUTPUT_BUFFER		65500	// max size from server to client
#define EASY_INLINE				static inline

typedef void*					ConnID;

#ifdef WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#define EASY_ASSERT(expr)	_ASSERT(expr)
	
	EASY_INLINE int32 easy_last_error()
	{
		return WSAGetLastError();
	}

#endif

#ifdef _LINUX
	#include <assert.h>
	#define EASY_ASSERT(expr)	assert(expr)
	#define SOCKET				int32
	#define closesocket			close
	#define INVALID_SOCKET		(-1)
	#define SOCKET_ERROR		(-1)
	#define __stdcall
	typedef sockaddr_in			SOCKADDR_IN;
	typedef sockaddr_in*		PSOCKADDR_IN;

	EASY_INLINE int32 easy_last_error()
	{
		return errno;
	}

#endif

//#define LOG_DBG(Expression, ...)	wprintf(Expression, ##__VA_ARGS__); wprintf(_T("\n"))
//#define LOG_ERR(Expression, ...)	wprintf(Expression, ##__VA_ARGS__); wprintf(_T("\n"))
//#define LOG_STT(Expression, ...)	wprintf(Expression, ##__VA_ARGS__); wprintf(_T("\n"))

#endif
