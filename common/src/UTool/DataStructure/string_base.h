#ifndef _H_STRING_BASE
#define _H_STRING_BASE

#include "array_base.h"

namespace Fantuan
{

template<typename T>
class StringBase
{
public:
	size_t		strlen() const
	{
		size_t len = 0;
		while (*(*m_pData)++)
		{
			len++;
		}

		return len;
	}

	const T*	c_str() const
	{
		return m_pData->data();
	}

protected:
	StringBase(ArrayBase<T>* pData) : m_pData(pData)
	{
	}

	void		assign(const T* pData)
	{
		size_t len = strlen(pData);
		size_t max_size = m_pData->max_size();
		len = len > max_size ? max_size : len;
		m_pData->assign(pData, len+1);
		(*m_pData)[len] = '\0';
	}

	size_t		strlen(const T* pData) const
	{
		size_t len = 0;
		while (*pData++)
		{
			len++;
		}

		return len;
	}

private:
	ArrayBase<T>* m_pData;
};

template<>
size_t	StringBase<char>::strlen()	const
{
	return ::strlen(m_pData->data());
}

template<>
size_t	StringBase<char>::strlen(const char* pData)	const
{
	return ::strlen(pData);
}

template<>
size_t	StringBase<wchar_t>::strlen()	const
{
	return ::wcslen(m_pData->data());
}

template<>
size_t	StringBase<wchar_t>::strlen(const wchar_t* pData)	const
{
	return ::wcslen(pData);
}


}

#endif
