#ifndef _H_BASE
#define _H_BASE

#ifdef WIN32
	#include "win32_base.h"
#endif

#ifdef _LINUX
	#include "linux_base.h"

	#define _aligned_malloc(s, a)	aligned_alloc(a, s)
	#define _aligned_free(s)		free(s)
	#define MEMORY_ALLOCATION_ALIGNMENT	8
#endif

#include "type.h"
#include <mutex>
#include <thread>
#include <chrono>

#define SAFE_DELETE(ptr)		if (ptr) { delete (ptr); (ptr) = nullptr; }
#define SAFE_DELETE_ARRAY(ptr)	if (ptr) { delete [] (ptr); (ptr) = NULL; }

#endif
