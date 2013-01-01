#ifndef _H_DS_AUX
#define _H_DS_AUX

#include "type_traits.h"

#ifdef WIN32
#pragma warning(push)
#pragma warning (disable: 4127)
#pragma warning (disable: 4996)	// evil std::copy check
#endif


#ifndef WIN32
#include <algorithm>
#endif

namespace Fantuan
{

// object swap
template<typename T>
inline void object_swap(T& lhs, T& rhs)
{
	std::swap(lhs, rhs);
}

// construct object
template<typename T>
inline void object_construct(T* ptr, const T& val)
{
	if (TypeTraits<T>::isScalar)
	{
		*ptr = val;
	}
	else if (TypeTraits<T>::isClass)
	{
		new ((void*)ptr) T(val);
	}
}

template<typename T>
inline void object_construct(T* ptr)
{
	if (TypeTraits<T>::isClass)
	{
		new ((void*)ptr) T;
	}
}

// destruct object
template<typename T>
inline void object_destruct(T* ptr)
{
	(ptr)->~T();
}

template<typename T>
inline void object_destruct(T* first, T* last)
{
	if (TypeTraits<T>::isClass)
	{
		for (; first != last; first += 1)
		{
			object_destruct(first);
		}
	}
}

// allocate object
template<typename T>
inline T* object_allocate(size_t count, T*)
{
	return (T*)::operator new(count * sizeof(T));
}

// free object
inline void object_free(void* ptr)
{
	::operator delete(ptr);
}

template<typename T>
inline T* object_uninit_copy(T* first, T* last, T* result)
{
	if (TypeTraits<T>::isScalar)
	{
		return std::copy(first, last, result);
	}
	else if (TypeTraits<T>::isClass)
	{
		T* curr = result;
		for (; first != last; first += 1, curr += 1)
		{
			object_construct(curr, *first);
		}

		return curr;
	}

	FT_ASSERT(false && "unknown type");
	return NULL;
}

}

#endif
