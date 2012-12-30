#ifndef _H_UTIL
#define _H_UTIL

#include "type.h"
#ifndef WIN32
#include <algorithm>
#endif

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
	//std::swap(lhs, const_cast<T&>(val));
	lhs = val;
}

// construct object
template<typename T>
inline void object_construct(T* ptr, const T& val)
{
	void *_Vptr = ptr;
	new (_Vptr) T(val);
}

// destruct object
template<typename T>
inline void object_destruct(T* ptr)
{
	(ptr)->~T();
}

// allocate object
template<typename T>
inline T* object_allocate(size_t count)
{
	return (T*)::operator new(count * sizeof(T));
}

// free object
inline void object_free(void* ptr)
{
	::operator delete(ptr);
}

}

#endif

