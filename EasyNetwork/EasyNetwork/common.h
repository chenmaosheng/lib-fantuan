#ifndef _H_COMMON
#define _H_COMMON

#ifdef WIN32
	#include "win32_base.h"
#endif

#ifdef _LINUX
	#include "linux_base.h"
#endif

#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <stdexcept>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include "type.h"

#define MAX_BUFFER	65536
#define MAXLINE		1024
#define SAFE_DELETE(ptr) if (ptr) { delete (ptr); (ptr) = nullptr; }

#ifdef WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#define EASY_ASSERT(expr)	_ASSERT(expr)
	#define LAST_ERROR			WSAGetLastError()

#endif

#ifdef _LINUX
	#include <assert.h>
	#define EASY_ASSERT(expr)	assert(expr)
	#define SOCKET				int32
	#define closesocket			close
	#define LAST_ERROR			errno
	#define INVALID_SOCKET		(-1)
	#define SOCKET_ERROR		(-1)

	typedef sockaddr_in	SOCKADDR_IN;

#endif



#endif
