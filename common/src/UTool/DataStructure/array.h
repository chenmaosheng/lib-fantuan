#ifndef _H_ARRAY
#define _H_ARRAY

#include "array_base.h"

namespace Fantuan
{

template<typename T, std::size_t N=ARRAY_MIN_SIZE>
class Array : public ArrayBase<T>
{
public:
	typedef ArrayBase<T>						baseClass;
	typedef Array<T, N>							thisClass;
	typedef typename baseClass::size_type		size_type;
	typedef typename baseClass::value_type		value_type;
	typedef typename baseClass::iterator		iterator;
	typedef typename baseClass::const_iterator	const_iterator;
	typedef typename baseClass::reference		reference;
	typedef typename baseClass::const_reference	const_reference;

	Array()
	{
		memset(m_Elements, 0, sizeof(m_Elements));
		baseClass::m_pHead = (T*)m_Elements;
	}

	inline reference		operator[](size_type i)
	{
		FT_ASSERT(i < size() && "out of range");
		return ((T*)m_Elements)[i];
	}

	inline const_reference	operator[](size_type i) const
	{
		FT_ASSERT(i < size() && "out of range");
		return ((T*)m_Elements)[i];
	}

	inline size_type		max_size() const
	{
		return N;
	}

	inline size_type		size() const
	{
		return N;
	}

	inline void				assign(const T* pData, size_type len)
	{
		memcpy(m_Elements, pData, sizeof(T)*len);
	}

private:
	char					m_Elements[N * sizeof(T)];
};

// template specification
template<typename T>
class Array<T, 0> : public ArrayBase<T>
{
public:
	typedef ArrayBase<T>						baseClass;
	typedef Array<T, 0>							thisClass;
	typedef typename baseClass::size_type		size_type;
	typedef typename baseClass::value_type		value_type;
	typedef typename baseClass::iterator		iterator;
	typedef typename baseClass::const_iterator	const_iterator;
	typedef typename baseClass::reference		reference;
	typedef typename baseClass::const_reference	const_reference;

	inline size_type		max_size() const
	{
		return 0;
	}

	inline size_type		size() const
	{
		return 0;
	}

	inline void				assign(const T* pData, size_type len)
	{
	}

	inline reference		operator[](size_type i)
	{
		return failed_rangecheck();
	}

	inline const_reference	operator[](size_type i) const
	{
		return failed_rangecheck();
	}

	// check range
	static reference failed_rangecheck()
	{
		std::out_of_range e("attempt to access element of an empty array");
		throw e;
		static T placeholder;
		return placeholder;
	}
};

}

#endif

