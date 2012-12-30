#ifndef _H_ARRAY
#define _H_ARRAY

#include "ft_assert.h"
#include "type_traits.h"

#ifdef WIN32
#pragma warning(push)
#pragma warning (disable: 4127)
#pragma warning (disable: 4996)	// evil std::copy check
#endif

#define ARRAY_MIN_SIZE		10
#define ARRAY_INC_FACTOR	2

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

	// iterator support
	inline iterator				begin()
	{
		return m_pElements;
	}

	inline const_iterator		begin() const
	{
		return m_pElements;
	}

	inline iterator				end()
	{
		return m_pElements + size();
	}

	inline const_iterator		end()	const
	{
		return m_pElements + size();
	}

	// operator[]
	inline reference			operator[](size_type i)
	{
		FT_ASSERT(i < size() && "out of range");
		return m_pElements[i];
	}

	inline const_reference		operator[](size_type i) const
	{
		FT_ASSERT(i < size() && "out of range");
		return m_pElements[i];
	}

	virtual size_type			max_size() const = 0;
	virtual size_type			size() const = 0;

protected:
	T*							m_pElements;
};

template<typename T, std::size_t N=ARRAY_MIN_SIZE>
class Array : public ArrayBase<T>
{
public:
	typedef ArrayBase<T>					baseClass;
	typedef typename baseClass::size_type	size_type;
	
	Array()
	{
		baseClass::m_pElements = m_Elements;
	}

	inline size_type	max_size() const
	{
		return N;
	}

	inline size_type	size() const
	{
		return N;
	}

private:
	T					m_Elements[N];
};

// template specification
template<typename T>
class Array<T, 0> : public ArrayBase<T>
{
public:
	typedef ArrayBase<T>						baseClass;
	typedef typename baseClass::iterator		iterator;
	typedef typename baseClass::const_iterator	const_iterator;
	typedef typename baseClass::size_type		size_type;
	typedef typename baseClass::reference		reference;
	typedef typename baseClass::const_reference	const_reference;

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

	// operator[]
	inline reference		operator[](size_type)
	{
		return failed_rangecheck();
	}

	inline const_reference	operator[](size_type) const
	{
		return failed_rangecheck();
	}

	inline size_type		max_size() const
	{
		return 0;
	}

	inline size_type		size() const
	{
		return 0;
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

template<typename T, std::size_t N=ARRAY_MIN_SIZE>
class DArray : public ArrayBase<T>
{
public:
	typedef ArrayBase<T>					baseClass;
	typedef typename baseClass::size_type	size_type;

	DArray() :	m_pDynamicElements(0), 
				m_iMaxSize(N), 
				m_iCount(0)
	{
		baseClass::m_pElements = m_Elements;
	}

	~DArray()
	{
		if (TypeTraits<T>::isClass)
		{
			for (size_type i = 0; i < m_iMaxSize; ++i)
			{
				(&m_pDynamicElements[i])->~T();
			}
		}

		SAFE_DELETE(m_pDynamicElements);
	}

	inline void				push_back(const T& value)
	{
		if (m_iCount < m_iMaxSize)
		{
			if (!m_pDynamicElements)
			{
				_insert(baseClass::m_pElements, value);
			}
			else
			{
				_insert(m_pDynamicElements, value);
			}
		}
		else
		{
			_grow(value);
		}

		++m_iCount;
	}

	inline size_type		max_size() const
	{
		if (!m_pDynamicElements)
		{
			return N;
		}

		return m_iMaxSize;
	}

	inline size_type		size() const
	{
		return m_iCount;
	}

private:
	void					_insert(T* pElements, const T& value)
	{
		if (TypeTraits<T>::isScalar)
		{
			pElements[m_iCount] = value;
		}
		else if (TypeTraits<T>::isClass)
		{
			object_construct(&pElements[m_iCount], value);
		}
	}

	void					_grow(const T& value)
	{
		size_type iOldSize = m_iMaxSize;
		m_iMaxSize *= ARRAY_INC_FACTOR;
		T* pData = object_allocate<T>(m_iMaxSize);

		if (TypeTraits<T>::isScalar)
		{
			if (!m_pDynamicElements)
			{
				std::copy(baseClass::m_pElements, baseClass::m_pElements + iOldSize, pData);	
			}
			else
			{
				std::copy(m_pDynamicElements, m_pDynamicElements + iOldSize, pData);
				SAFE_DELETE(m_pDynamicElements);
			}

			m_pDynamicElements = pData;
			m_pDynamicElements[m_iCount] = value;
		}
		else if (TypeTraits<T>::isClass)
		{
			// add new stuff
			object_construct(&pData[m_iCount], value);
			// move old stuff
			if (!m_pDynamicElements)
			{
				for (size_type i = 0; i < iOldSize; ++i)
				{
					object_construct(&pData[i], baseClass::m_pElements[i]);
				}
			}
			else
			{
				for (size_type i = 0; i < iOldSize; ++i)
				{
					object_construct(&pData[i], m_pDynamicElements[i]);
					object_swap(pData[i], m_pDynamicElements[i]);
					object_destruct(&m_pDynamicElements[i]);
				}

				object_free(m_pDynamicElements);
			}

			m_pDynamicElements = pData;
		}
	}

private:
	T			m_Elements[N];
	T*			m_pDynamicElements;
	size_type	m_iMaxSize;
	size_type	m_iCount;
};

template<typename T>
class DArray<T, 0> : public ArrayBase<T>
{
public:
	typedef ArrayBase<T>					baseClass;
	typedef typename baseClass::size_type	size_type;

	inline size_type	max_size() const
	{
		return 0;
	}

	inline size_type	size() const
	{
		return 0;
	}
};

}

#ifdef WIN32
#pragma warning(pop)
#endif

#endif

