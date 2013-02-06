#ifndef _H_UTIL
#define _H_UTIL

#include "type.h"

namespace Fantuan
{

#define SAFE_DELETE(ptr)		if (ptr) { delete (ptr); (ptr) = NULL; }
#define SAFE_DELETE_ARRAY(ptr)	if (ptr) { delete [] (ptr); (ptr) = NULL; }
#define ROUND_UP(value, mul)	(((value + mul - 1) &~(mul - 1)))

#ifdef WIN32
#define DATA_OFFSET(type, field) FIELD_OFFSET(type, field)
#else
#define DATA_OFFSET(type, field) offsetof(type, field)
#endif

// cacluate the 2nd power
uint8 NumberPower(uint32 iNumber);

// case sensitive transform

}

#endif

