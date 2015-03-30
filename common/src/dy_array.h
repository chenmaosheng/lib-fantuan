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

#define ARRAY_INC_FACTOR	2

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
				memcpy(m_Elements, rhs.m_Elements, sizeof(T)*_size);
				//std::copy((T*)rhs.m_Elements, (T*)rhs.m_Elements + _size, (T*)m_Elements);
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
				memcpy(m_pDynamicElements, rhs.m_pDynamicElements, sizeof(T)*_size);
				//std::copy(rhs.m_pDynamicElements, rhs.m_pDynamicElements + _size, m_pDynamicElements);
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
		if (std::is_class<T>::value)
		//if (TypeTraits<T>::isClass)
		{
			if (m_pDynamicElements)
			{
				destroy(begin(), end());
			}
		}
		
		object_free(m_pDynamicElements);
	}

	inline void			push_back(size_type n, const T& value)
	{
		size_type extra = 0;
		if (m_iCount + n > m_iMaxSize)
		{
			extra = m_iCount + n - m_iMaxSize;
		}

		if (m_iMaxSize > m_iCount)
		{
			_insert_n(m_iMaxSize - m_iCount, value);
		}
		
		if (extra != 0)
		{
			_grow_n(extra, value);
		}

		m_iCount += n;
	}

	inline void			push_back(const T& value)
	{
		push_back(1, value);
	}

	inline void			erase(const_iterator value)
	{
		iterator _where = const_cast<iterator>(value);
		if (TypeTraits<T>::isScalar)
		{
			memmove(_where, _where + 1, sizeof(T)*(end() - _where - 1));
			//std::copy(_where + 1, end(), _where);
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
			memmove(_first, _last, sizeof(T)*(end() - _last));
			//std::copy(_last, end(), _first);
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

	inline bool			empty() const
	{
		return m_iCount == 0;
	}

	inline void			assign(const T* pData, size_type len)
	{
		if (len >= m_iMaxSize)
		{
			resize(len);
		}

		memset(m_pHead, 0, m_iMaxSize);
		memcpy(m_pHead, pData, sizeof(T)*len);
		m_iCount = len;
	}

	inline void			append(const T* pData, size_type len, size_type pos)
	{
		if (len + m_iCount >= m_iMaxSize)
		{
			resize(len + m_iCount);
		}

		memcpy(m_pHead + pos, pData, sizeof(T)*len);
		m_iCount += len;
	}

	inline void			copy(const T* pData, size_type len)
	{
		if (len >= m_iMaxSize)
		{
			resize(len);
		}

		memcpy(m_pHead, pData, sizeof(T)*len);
		m_iCount = m_iCount > len ? m_iCount : len;
	}

	inline void			resize(size_type newSize)
	{
		if (newSize <= m_iCount)
		{
			m_iCount = newSize;
		}
		else
		{
			push_back(newSize - m_iCount, T());
		}
	}

private:
	void				_insert_n(size_type n, const T& value)
	{
		object_fill_n(&m_pHead[m_iCount], n, value);
	}

	void				_insert(const T& value)
	{
		_insert_n(1, value);
	}

	void				_buy(size_type newSize)
	{
		m_pDynamicElements = object_allocate(newSize, (T*)0);
	}

	void				_grow_n(size_type n, const T& value)
	{
		size_type iOldSize = m_iMaxSize;
		m_iMaxSize += n;
		T* pData = object_allocate(m_iMaxSize, (T*)0);

		if (TypeTraits<T>::isScalar)
		{
			if (!m_pDynamicElements)
			{
				memcpy(pData, m_Elements, sizeof(T)*iOldSize);
				//std::copy((T*)m_Elements, (T*)m_Elements + iOldSize, pData);	
			}
			else
			{
				memcpy(pData, m_pDynamicElements, sizeof(T)*iOldSize);
				//std::copy(m_pDynamicElements, m_pDynamicElements + iOldSize, pData);
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

	void				_grow(const T& value)
	{
		_grow_n(m_iMaxSize / ARRAY_INC_FACTOR >= 1 ? m_iMaxSize / ARRAY_INC_FACTOR : 1, value);
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

	inline void			assign(const T* pData, size_type len)
	{
	}

	inline void			append(const T* pData, size_type len, size_type pos)
	{
	}

	inline void			copy(const T* pData, size_type len)
	{
	}

	inline void			resize(size_type newSize)
	{
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

#ifdef WIN32
#pragma warning(pop)
#endif

#endif

