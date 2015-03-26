#ifndef _H_COMMON
#define _H_COMMON

#ifdef WIN32
	#include "win32_common.h"
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

#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define FT_ASSERT(expr) _ASSERT(expr)

#else
#include <assert.h>
#define FT_ASSERT(expr) assert(expr)
#endif

#define MAX_BUFFER 65536

#endif
