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
		// todo: efficiency
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


}

#endif
