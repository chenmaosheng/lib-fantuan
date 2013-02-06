#ifndef _H_ARRAY
#define _H_ARRAY

#include "ft_assert.h"

namespace Fantuan
{

template<typename T, std::size_t N=ARRAY_MIN_SIZE>
class Array
{
public:
	typedef T				value_type;
	typedef T*				iterator;
	typedef const T*		const_iterator;
	typedef T&				reference;
	typedef const T&		const_reference;
	typedef std::size_t		size_type;

	// iterator support
	inline iterator			begin()
	{
		return m_Elements;
	}

	inline const_iterator	begin() const
	{
		return m_Elements;
	}

	inline iterator			end()
	{
		return (T*)m_Elements + size();
	}

	inline const_iterator	end() const
	{
		return (T*)m_Elements + size();
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

	inline void*			data() const
	{
		return (void*)m_Elements;
	}

private:
	char					m_Elements[N * sizeof(T)];
};

// template specification
template<typename T>
class Array<T, 0>
{
public:
	typedef T				value_type;
	typedef T*				iterator;
	typedef const T*		const_iterator;
	typedef T&				reference;
	typedef const T&		const_reference;
	typedef std::size_t		size_type;
	// iterator support
	inline iterator			begin()
	{
		return iterator( reinterpret_cast<T*>(this) );
	}

	inline const_iterator	begin() const
	{
		return iterator( reinterpret_cast<const T*>(this) );
	}

	inline iterator			end()
	{
		return begin();
	}

	inline const_iterator	end()	const
	{
		return begin();
	}

	inline size_type		max_size() const
	{
		return 0;
	}

	inline size_type		size() const
	{
		return 0;
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

