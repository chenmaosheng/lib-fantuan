#ifndef _H_BASE
#define _H_BASE

#ifdef WIN32
	#include "win32_base.h"

	#define EASY_THREADID	GetCurrentThreadId()
	#define FUNCTION_NAME	_T(__FUNCTION__)
#endif

#ifdef _LINUX
	#include "linux_base.h"

	#define _aligned_malloc(s, a)	memalign(a, s)
	#define _aligned_free(s)		free(s)
	#define MEMORY_ALLOCATION_ALIGNMENT	8
	#define _T(x)					L ## x

	#define EASY_THREADID	syscall(SYS_gettid)
	#define FUNCTION_NAME	__FUNCTION__
#endif

#include "type.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <atomic>

#define SAFE_DELETE(ptr)		if (ptr) { delete (ptr); (ptr) = nullptr; }
#define SAFE_DELETE_ARRAY(ptr)	if (ptr) { delete [] (ptr); (ptr) = NULL; }

#endif
