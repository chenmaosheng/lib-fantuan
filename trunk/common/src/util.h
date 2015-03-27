#ifndef _H_UTIL
#define _H_UTIL

#include "type.h"

namespace Fantuan
{


#define ROUND_UP(value, mul)	(((value + mul - 1) &~(mul - 1)))

// cacluate the 2nd power
uint8 NumberPower(uint32 iNumber);

// case sensitive transform

}

#endif

