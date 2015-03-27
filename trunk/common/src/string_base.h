#ifndef _H_STRING_BASE
#define _H_STRING_BASE

#include "array_base.h"

namespace Fantuan
{

template<typename T>
class StringBase
{
public:
	inline StringBase& operator=(const T* pData)
	{
		assign(pData);
		return *this;
	}

	inline StringBase& operator=(const StringBase& rhs)
	{
		assign(rhs.c_str());
		return *this;
	}

	inline StringBase& operator+=(const T* pData)
	{
		append(pData);
		return *this;
	}

	inline StringBase& operator+=(const StringBase& rhs)
	{
		append(rhs.c_str());
		return *this;
	}

	inline bool operator>(const StringBase& rhs)	const
	{
		return strcmp(rhs.c_str()) > 0;
	}

	inline bool operator>(const T* pData) const
	{
		return strcmp(pData) > 0;
	}

	inline bool operator>=(const StringBase& rhs) const
	{
		return strcmp(rhs.c_str()) >= 0;
	}

	inline bool operator>=(const T* pData) const
	{
		return strcmp(pData) >= 0;
	}

	inline bool operator==(const StringBase& rhs) const
	{
		return equal(rhs.c_str());
	}

	inline bool operator==(const T* pData) const
	{
		return equal(pData);
	}

	inline bool operator<(const StringBase& rhs)	const
	{
		return strcmp(rhs.c_str()) < 0;
	}

	inline bool operator<(const T* pData) const
	{
		return strcmp(pData) < 0;
	}

	inline bool operator<=(const StringBase& rhs) const
	{
		return strcmp(rhs.c_str()) <= 0;
	}

	inline bool operator<=(const T* pData) const
	{
		return strcmp(pData) <= 0;
	}

	void		strcpy(const T* pData, size_t num)
	{
		m_pData->copy(pData, num);
	}

	void		strcpy(StringBase<T>& rhs)
	{
		strcpy(rhs.c_str(), rhs.strlen()+1);
	}

	void		clear()
	{
		m_pData->clear();
	}

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

	void		assign(const T* pData, size_t num)
	{
		m_pData->assign(pData, num);
	}

	void		assign(const T* pData)
	{
		assign(pData, strlen(pData)+1);
	}

	void		append(const T* pData, size_t num)
	{
		m_pData->append(pData, num, strlen());
	}

	void		append(const T* pData)
	{
		append(pData, strlen(pData)+1);
	}

	int32		strcmp(const T* pData)
	{
		const T* lhs = m_pData->data();
		for (;; *lhs++, *pData++)
		{
			if (*lhs != *pData)
			{
				return (*lhs - *pData);
			}

			if (*lhs == 0 || *pData == 0)
			{
				break;
			}
		}

		return 0;
	}

	bool		equal(const T* pData)
	{
		return strcmp(pData) == 0;
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

template<>
int32	StringBase<char>::strcmp(const char* pData)
{
	return ::strcmp(m_pData->data(), pData);
}

template<>
int32	StringBase<wchar_t>::strcmp(const wchar_t* pData)
{
	return ::wcscmp(m_pData->data(), pData);
}


}

#endif
