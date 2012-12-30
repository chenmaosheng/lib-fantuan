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
	iterator			begin()
	{
		return m_pElements;
	}

	const_iterator		begin() const
	{
		return m_pElements;
	}

	iterator			end()
	{
		return m_pElements + size();
	}

	const_iterator		end()	const
	{
		return m_pElements + size();
	}

	// operator[]
	reference			operator[](size_type i)
	{
		FT_ASSERT(i < size() && "out of range");
		return m_pElements[i];
	}

	const_reference		operator[](size_type i) const
	{
		FT_ASSERT(i < size() && "out of range");
		return m_pElements[i];
	}

	virtual size_type			max_size() const = 0;
	virtual size_type			size() const = 0;

protected:
	T*			m_pElements;
};

template<typename T, std::size_t N=ARRAY_MIN_SIZE>
class Array : public ArrayBase<T>
{
public:
	typedef ArrayBase<T> baseClass;
	typedef typename baseClass::size_type size_type;
	
	Array()
	{
		baseClass::m_pElements = m_Elements;
	}

	size_type		max_size() const
	{
		return N;
	}

	size_type		size() const
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
	typedef ArrayBase<T> baseClass;
	typedef typename baseClass::iterator iterator;
	typedef typename baseClass::const_iterator const_iterator;
	typedef typename baseClass::size_type size_type;
	typedef typename baseClass::reference reference;
	typedef typename baseClass::const_reference const_reference;

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

	size_type		max_size() const
	{
		return 0;
	}

	size_type		size() const
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
	typedef ArrayBase<T>	baseClass;
	typedef typename baseClass::size_type	size_type;

	DArray() : m_pDynamicElements(0), m_iMaxSize(N), m_iCount(0)
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

	void			push_back(const T& value)
	{
		if (!m_pDynamicElements)
		{
			if (m_iCount < m_iMaxSize)
			{
				if (TypeTraits<T>::isScalar)
				{
					baseClass::m_pElements[m_iCount] = value;
				}
				else if (TypeTraits<T>::isClass)
				{
					object_swap(baseClass::m_pElements[m_iCount], value);
				}
			}
			else
			{
				m_iMaxSize = N * ARRAY_INC_FACTOR;
				T* pData = (T*)::operator new(sizeof(T) * m_iMaxSize);

				if (TypeTraits<T>::isScalar)
				{
					m_pDynamicElements = (T*)pData;
					std::copy(baseClass::m_pElements, baseClass::m_pElements + N, m_pDynamicElements);
					m_pDynamicElements[m_iCount] = value;
				}
				else if (TypeTraits<T>::isClass)
				{
					for (size_type i = 0; i < m_iMaxSize; ++i)
					{
						new (&pData[i]) T;
					}

					m_pDynamicElements = pData;

					for (size_type i = 0; i < N; ++i)
					{
						object_swap(m_pDynamicElements[i], baseClass::m_pElements[i]);
					}
					object_swap(m_pDynamicElements[m_iCount], value);
				}
			}
		}
		else
		{
			if (m_iCount < m_iMaxSize)
			{
				if (TypeTraits<T>::isScalar)
				{
					m_pDynamicElements[m_iCount] = value;
				}
				else if (TypeTraits<T>::isClass)
				{
					object_swap(m_pDynamicElements[m_iCount], value);
				}
			}
			else
			{
				size_type iOldSize = m_iMaxSize;
				m_iMaxSize *= ARRAY_INC_FACTOR;
				T* pData = (T*)::operator new(sizeof(T) * m_iMaxSize);
				
				if (TypeTraits<T>::isScalar)
				{
					std::copy(m_pDynamicElements, m_pDynamicElements + iOldSize, pData);
					SAFE_DELETE(m_pDynamicElements);
					m_pDynamicElements = pData;
					m_pDynamicElements[m_iCount] = value;
				}
				else if (TypeTraits<T>::isClass)
				{
					for (size_type i = 0; i < m_iMaxSize; ++i)
					{
						new (&pData[i]) T;
					}

					for (size_type i = 0; i < iOldSize; ++i)
					{
						object_swap(pData[i], m_pDynamicElements[i]);
						(&m_pDynamicElements[i])->~T();
					}

					SAFE_DELETE(m_pDynamicElements);
					m_pDynamicElements = pData;
					object_swap(m_pDynamicElements[m_iCount], value);
				}
			}
		}

		++m_iCount;
	}

	size_type		max_size() const
	{
		if (!m_pDynamicElements)
		{
			return N;
		}

		return m_iMaxSize;
	}

	size_type		size() const
	{
		return m_iCount;
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
	typedef ArrayBase<T> baseClass;
	typedef typename baseClass::size_type size_type;

	size_type		max_size() const
	{
		return 0;
	}

	size_type		size() const
	{
		return 0;
	}
};

}

#ifdef WIN32
#pragma warning(pop)
#endif

#endif

