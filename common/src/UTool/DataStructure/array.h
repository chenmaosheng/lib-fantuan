#ifndef _H_ARRAY
#define _H_ARRAY

#include "ft_assert.h"

namespace Fantuan
{

template<typename T, std::size_t N, bool bAllowDynamic=true>
class Array
{
public:
	// type definitions
	typedef T			value_type;
	typedef T*			iterator;
	typedef const T*	const_iterator;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef std::size_t	size_type;

	static const bool	s_bAllowDynamic = bAllowDynamic;

	// iterator support
	iterator			begin()
	{
		return m_Elements;
	}

	const_iterator		begin() const
	{
		return m_Elements;
	}

	iterator			end()
	{
		return m_Elements + N;
	}

	const_iterator		end()	const
	{
		return m_Elements + N;
	}

	// operator[]
	reference			operator[](size_type i)
	{
		FT_ASSERT(i < N && "out of range");
		return m_Elements[i];
	}

	const_reference		operator[](size_t i) const
	{
		FT_ASSERT(i < N && "out of range");
		return m_Elements[i];
	}

	void				push_back(T* pElement)
	{

	}

	static size_type	size()
	{
		return N;
	}

	static bool			empty()
	{
		return false;
	}

protected:
	T					m_Elements[N];
	
private:
	T*					m_pDynamicElements;
};

// template specification
template<typename T>
class Array<T, 0>
{
public:
	// type definitions
	typedef T			value_type;
	typedef T*			iterator;
	typedef const T*	const_iterator;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef std::size_t	size_type;

	// iterator support
	iterator		begin()
	{
		return iterator( reinterpret_cast<T*>(this) );
	}

	const_iterator	begin() const
	{
		return iterator( reinterpret_cast<const T*>(this) );
	}

	iterator		end()
	{
		return begin();
	}

	const_iterator	end()	const
	{
		return begin();
	}

	// operator[]
	reference operator[](size_type)
	{
		return failed_rangecheck();
	}

	const_reference operator[](size_type) const
	{
		return failed_rangecheck();
	}

	static size_type	size()
	{
		return 0;
	}

	static bool			empty()
	{
		return true;
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

