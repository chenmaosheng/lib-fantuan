#ifndef _H_ARRAY_BASE
#define _H_ARRAY_BASE

#include "ft_assert.h"

namespace Fantuan
{

template<typename T>
class ArrayBase
{
public:
	// type definitions
	typedef T			value_type;
	typedef T*			iterator;
	typedef const T*	const_iterator;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef std::size_t	size_type;

	ArrayBase() : m_pHead(0)
	{

	}

	// iterator support
	inline iterator			begin()
	{
		return m_pHead;
	}

	inline const_iterator	begin() const
	{
		return m_pHead;
	}

	inline iterator			end()
	{
		return m_pHead + size();
	}

	inline const_iterator	end() const
	{
		return m_pHead + size();
	}

	// operator[]
	inline reference		operator[](size_type i)
	{
		FT_ASSERT(i < size() && "out of range");
		return m_pHead[i];
	}

	inline const_reference	operator[](size_type i) const
	{
		FT_ASSERT(i < size() && "out of range");
		return m_pHead[i];
	}

	inline T*				operator++(int)
	{
		T* pHead = m_pHead;
		m_pHead += 1;
		return pHead;
	}

	inline T*				operator++()
	{
		m_pHead += 1;
		return m_pHead;
	}

	inline T*				data() const
	{
		return m_pHead;
	}

	inline T*				data()
	{
		return m_pHead;
	}

	inline void				clear()
	{
		memset(m_pHead, 0, max_size());
	}
	
	virtual size_type		max_size() const = 0;
	virtual size_type		size() const = 0;
	virtual void			assign(const T* pData, size_type len) = 0;
	virtual void			copy(const T* pData, size_type len) = 0;
	virtual void			resize(size_type newSize) = 0;
	
protected:
	T*						m_pHead;
};

}

#endif
