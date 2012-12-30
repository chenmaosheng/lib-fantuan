#ifndef _H_DY_ARRAY
#define _H_DY_ARRAY

#include "ft_assert.h"
#include "type_traits.h"

#ifdef WIN32
#pragma warning(push)
#pragma warning (disable: 4127)
#pragma warning (disable: 4996)	// evil std::copy check
#endif

#define ARRAY_MIN_SIZE		1
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
	virtual iterator			begin() = 0;
	virtual const_iterator		begin() const = 0;
	virtual iterator			end() = 0;
	virtual const_iterator		end() const = 0;

	// operator[]
	virtual reference			operator[](size_type i) = 0;
	virtual const_reference		operator[](size_type i) const = 0;

	virtual size_type			max_size() const = 0;
	virtual size_type			size() const = 0;
};

template<typename T, std::size_t N=ARRAY_MIN_SIZE>
class DyArray : public ArrayBase<T>
{
public:
	typedef ArrayBase<T>						baseClass;
	typedef DyArray<T, N>						thisClass;
	typedef typename baseClass::size_type		size_type;
	typedef typename baseClass::value_type		value_type;
	typedef typename baseClass::iterator		iterator;
	typedef typename baseClass::const_iterator	const_iterator;
	typedef typename baseClass::reference		reference;
	typedef typename baseClass::const_reference	const_reference;

	DyArray() :	m_pDynamicElements(0), 
				m_iMaxSize(N), 
				m_iCount(0)
	{
	}

	DyArray(const thisClass& rhs) :	m_pDynamicElements(0),
									m_iMaxSize(N),
									m_iCount(0)
	{
		_buy(rhs.max_size());
		size_type _size = rhs.size();
		if (TypeTraits<T>::isScalar)
		{
			if (!rhs.m_pDynamicElements)
			{
				std::copy(rhs.m_Elements, rhs.m_Elements + _size, m_Elements);	
			}
			else
			{
				std::copy(rhs.m_pDynamicElements, rhs.m_pDynamicElements + _size, m_pDynamicElements);
			}
		}
		else if (TypeTraits<T>::isClass)
		{
			if (!rhs.m_pDynamicElements)
			{
				for (size_type i = 0; i < _size; ++i)
				{
					object_construct(&m_Elements[i], rhs.m_Elements[i]);
				}
			}
			else
			{
				for (size_type i = 0; i < _size; ++i)
				{
					object_construct(&m_pDynamicElements[i], rhs.m_pDynamicElements[i]);
				}
			}
		}
	}

	//thisClass& operator=(const thisClass& rhs)
	//{
	//	
	//}

	~DyArray()
	{
		if (TypeTraits<T>::isClass)
		{
			for (size_type i = 0; i < m_iCount; ++i)
			{
				if (m_pDynamicElements)
				{
					(&m_pDynamicElements[i])->~T();
				}
				else
				{
					(&m_Elements[i])->~T();
				}
			}

			m_iCount = 0;
		}
		
		SAFE_DELETE(m_pDynamicElements);
	}

	inline void			push_back(const T& value)
	{
		if (m_iCount < m_iMaxSize)
		{
			if (!m_pDynamicElements)
			{
				_insert(m_Elements, value);
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

	inline void			erase(const_iterator value)
	{
		iterator _where = const_cast<iterator>(value);
		if (TypeTraits<T>::isScalar)
		{
			std::copy(_where + 1, end(), _where);
		}
		else
		{
			iterator it = _where + 1;
			for (; it != end(); it += 1)
			{
				object_construct(it - 1, *it); 
			}
			object_destruct(it - 1);
		}

		--m_iCount;
	}

	// iterator operation
	inline iterator		begin()
	{
		if (!m_pDynamicElements)
		{
			return m_Elements;
		}

		return m_pDynamicElements;
	}

	inline const_iterator	begin() const
	{
		if (!m_pDynamicElements)
		{
			return m_Elements;
		}

		return m_pDynamicElements;
	}

	inline iterator			end()
	{
		if (!m_pDynamicElements)
		{
			return m_Elements + size();
		}

		return m_pDynamicElements + size();
	}

	inline const_iterator	end() const
	{
		if (!m_pDynamicElements)
		{
			return m_Elements + size();
		}

		return m_pDynamicElements + size();
	}

	// operator[]
	inline reference		operator[](size_type i)
	{
		FT_ASSERT(i < size() && "out of range");
		if (!m_pDynamicElements)
		{
			return m_Elements[i];
		}

		return m_pDynamicElements[i];
	}

	inline const_reference	operator[](size_type i) const
	{
		FT_ASSERT(i < size() && "out of range");
		if (!m_pDynamicElements)
		{
			return m_Elements[i];
		}

		return m_pDynamicElements[i];
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

	void					_buy(size_type newSize)
	{
		if (newSize > max_size())
		{
			m_pDynamicElements = object_allocate(newSize, (T*)0);
		}
	}

	void					_grow(const T& value)
	{
		size_type iOldSize = m_iMaxSize;
		m_iMaxSize += m_iMaxSize / ARRAY_INC_FACTOR >= 1 ? m_iMaxSize / ARRAY_INC_FACTOR : 1;
		T* pData = object_allocate(m_iMaxSize, (T*)0);

		if (TypeTraits<T>::isScalar)
		{
			if (!m_pDynamicElements)
			{
				std::copy(m_Elements, m_Elements + iOldSize, pData);	
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
					object_construct(&pData[i], m_Elements[i]);
				}
			}
			else
			{
				value_type emptyVal;
				for (size_type i = 0; i < iOldSize; ++i)
				{
					object_construct(&pData[i], emptyVal);
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
class DyArray<T, 0> : public ArrayBase<T>
{
public:
	typedef ArrayBase<T>						baseClass;
	typedef DyArray<T, 0>						thisClass;
	typedef typename baseClass::size_type		size_type;
	typedef typename baseClass::value_type		value_type;
	typedef typename baseClass::iterator		iterator;
	typedef typename baseClass::const_iterator	const_iterator;
	typedef typename baseClass::reference		reference;
	typedef typename baseClass::const_reference	const_reference;
	
	inline size_type	max_size() const
	{
		return 0;
	}

	inline size_type	size() const
	{
		return 0;
	}

	inline reference			operator[](size_type i)
	{
		return failed_rangecheck();
	}

	inline const_reference		operator[](size_type i) const
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

#ifdef WIN32
#pragma warning(pop)
#endif

#endif

