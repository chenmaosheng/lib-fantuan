#ifndef _H_ASSERT
#define _H_ASSERT

#ifdef WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>

	#define FT_ASSERT(expr) _ASSERT(expr)

#else
	#include <assert.h>

	#define FT_ASSERT(expr) assert(expr)
#endif

#ifdef _DEBUG
	#define FT_VERIFY(expr) FT_ASSERT(expr)
#else
	#define FT_VERIFY(expr) ((void)(expr))
#endif

#endif

