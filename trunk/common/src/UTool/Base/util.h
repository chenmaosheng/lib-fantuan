#ifndef _H_UTIL
#define _H_UTIL

#include "type.h"

namespace Fantuan
{

#define SAFE_DELETE(ptr)		if (ptr) { delete (ptr); (ptr) = NULL; }
#define SAFE_DELETE_ARRAY(ptr)	if (ptr) { delete [] (ptr); (ptr) = NULL; }

// cacluate the 2nd power
uint8 NumberPower(uint32 iNumber);

// case sensitive transform


// object swap
template<typename T>
inline void object_swap(T& lhs, const T& val)
{
	//T temp(val);
	std::swap(lhs, const_cast<T&>(val));
}

}

#endif

