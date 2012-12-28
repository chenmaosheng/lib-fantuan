#ifndef _H_BASE
#define _H_BASE

#ifdef WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#include "win32_base.h"
#endif

#ifdef _LINUX
	#include "linux_base.h"
#endif

#include "type.h"
#include "util.h"

#endif

