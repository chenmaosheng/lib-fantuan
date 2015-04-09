#ifndef _H_COMMON
#define _H_COMMON

#include "base.h"
#include "easy_log.h"

#define MAXLINE					256
#define MAX_INPUT_BUFFER		1024	// max size from client to server
#define MAX_OUTPUT_BUFFER		65500	// max size from server to client
#define MAX_SESSION				32768
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

	#define easy_sleep(x)	Sleep(x)

#endif

#ifdef _LINUX
	#include <assert.h>
	#define EASY_ASSERT(expr)	assert(expr)
	#define SOCKET				int32
	#define closesocket			close
	#define INVALID_SOCKET		(-1)
	#define SOCKET_ERROR		(-1)
	#define MAX_EVENT			8192
	#define __stdcall
	typedef sockaddr_in			SOCKADDR_IN;
	typedef sockaddr_in*		PSOCKADDR_IN;

	EASY_INLINE int32 easy_last_error()
	{
		return errno;
	}

	EASY_INLINE uint32	timeGetTime()
	{
		timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
	}

	#define easy_sleep(x)	usleep(x * 1000)

#endif

#endif
