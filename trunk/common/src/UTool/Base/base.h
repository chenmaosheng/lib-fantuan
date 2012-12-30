#ifndef _H_BASE
#define _H_BASE

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

#include "type.h"
#include "util.h"
#include "ft_assert.h"
#include "smart_ptr.h"
#include "accurate_time.h"
#include "type_list.h"
#include "type_traits.h"

#endif

