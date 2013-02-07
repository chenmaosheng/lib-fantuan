#ifndef _H_DY_ARRAY
#define _H_DY_ARRAY

#include "type_traits.h"
#include "ds_aux.h"
#include "array_base.h"
#include <stdexcept>

#ifdef WIN32
#pragma warning(push)
#pragma warning (disable: 4127)
#pragma warning (disable: 4996)	// evil std::copy check
#endif

#define ARRAY_MIN_SIZE		1
#define ARRAY_INC_FACTOR	2

namespace Fantuan
{


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

	using baseClass::m_pHead;
	using baseClass::begin;
	using baseClass::end;

	DyArray() :	m_pDynamicElements(0), 
				m_iMaxSize(N), 
				m_iCount(0)
	{
		memset(m_Elements, 0, sizeof(m_Elements));
		m_pHead = (T*)m_Elements;
	}

	DyArray(const thisClass& rhs) :	m_pDynamicElements(0),
									m_iMaxSize(N),
									m_iCount(0)
	{
		memset(m_Elements, 0, sizeof(m_Elements));
		size_type _size = rhs.size();

		if (!rhs.m_pDynamicElements)
		{
			if (TypeTraits<T>::isScalar)
			{
				std::copy((T*)rhs.m_Elements, (T*)rhs.m_Elements + _size, (T*)m_Elements);
			}
			else if (TypeTraits<T>::isClass)
			{
				for (size_type i = 0; i < _size; ++i)
				{
					object_construct(&((T*)m_Elements)[i], ((T*)rhs.m_Elements)[i]);
				}
			}
			m_pHead = (T*)m_Elements;
		}
		else
		{
			_buy(rhs.max_size());
			if (TypeTraits<T>::isScalar)
			{
				std::copy(rhs.m_pDynamicElements, rhs.m_pDynamicElements + _size, m_pDynamicElements);
			}
			else if (TypeTraits<T>::isClass)
			{
				for (size_type i = 0; i < _size; ++i)
				{
					object_construct(&m_pDynamicElements[i], rhs.m_pDynamicElements[i]);
				}
			}
			m_pHead = m_pDynamicElements;
		}
	}

	~DyArray()
	{
		if (TypeTraits<T>::isClass)
		{
			if (m_pDynamicElements)
			{
				destroy(begin(), end());
			}
		}
		
		object_free(m_pDynamicElements);
	}

	inline void			push_back(const T& value)
	{
		if (m_iCount < m_iMaxSize)
		{
			_insert(m_pHead, value);
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

	inline void			erase(const_iterator first, const_iterator last)
	{
		iterator _first = const_cast<iterator>(first);
		iterator _last = const_cast<iterator>(last);
	
		if (_first > _last || end() < _last)
		{
			FT_ASSERT(false && "out of range");
			return;
		}

		size_type _off = _last - _first;

		if (TypeTraits<T>::isScalar)
		{
			std::copy(_last, end(), _first);
		}
		else if (TypeTraits<T>::isClass)
		{
			for(; _last != end(); _last += 1, _first += 1)
			{
				object_construct(_first, *_last);
				object_destruct(_last);
			}
		}

		m_iCount -= _off;
	}

	inline void			clear()
	{
		erase(begin(), end());
	}

	inline void			destroy(const_iterator first, const_iterator last)
	{
		iterator _first = const_cast<iterator>(first);
		iterator _last = const_cast<iterator>(last);

		size_type _off = _last - _first;
	
		if (_first > _last || end() < _last)
		{
			FT_ASSERT(false && "out of range");
			return;
		}

		if (TypeTraits<T>::isScalar)
		{
			memset((void*)first, 0, sizeof(T)*_off);
		}
		else if (TypeTraits<T>::isClass)
		{
			for (; _first != _last; _first += 1)
			{
				object_destruct(_first);
			}
		}
	}

	inline size_type	max_size() const
	{
		return m_iMaxSize;
	}

	inline size_type	size() const
	{
		return m_iCount;
	}

private:
	void				_insert(T* pElements, const T& value)
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

	void				_buy(size_type newSize)
	{
		m_pDynamicElements = object_allocate(newSize, (T*)0);
	}

	void				_grow(const T& value)
	{
		size_type iOldSize = m_iMaxSize;
		m_iMaxSize += m_iMaxSize / ARRAY_INC_FACTOR >= 1 ? m_iMaxSize / ARRAY_INC_FACTOR : 1;
		T* pData = object_allocate(m_iMaxSize, (T*)0);

		if (TypeTraits<T>::isScalar)
		{
			if (!m_pDynamicElements)
			{
				std::copy((T*)m_Elements, (T*)m_Elements + iOldSize, pData);	
			}
			else
			{
				std::copy(m_pDynamicElements, m_pDynamicElements + iOldSize, pData);
				object_free(m_pDynamicElements);
			}

			m_pDynamicElements = pData;
			m_pHead = m_pDynamicElements;
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
					object_construct(&pData[i], ((T*)m_Elements)[i]);
				}
			}
			else
			{
				for (size_type i = 0; i < iOldSize; ++i)
				{
					object_construct(&pData[i]);
					// default swap need another one construct, one construct means one destruct, attention!
					object_swap(pData[i], m_pDynamicElements[i]);
					object_destruct(&m_pDynamicElements[i]);
				}

				object_free(m_pDynamicElements);
			}

			m_pDynamicElements = pData;
			m_pHead = m_pDynamicElements;
		}
	}

private:
	char		m_Elements[N * sizeof(T)];
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

	inline reference	operator[](size_type i)
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

