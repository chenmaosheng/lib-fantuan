#ifndef _H_LOCAL_STRING
#define _H_LOCAL_STRING

#include "string_base.h"
#include "array.h"

namespace Fantuan
{

template<typename T, size_t size>
class FTLocalString : public StringBase<T>
{
public:
	FTLocalString() : StringBase<T>(&m_Buffer)
	{
	}

	FTLocalString(const T* pData) : StringBase<T>(&m_Buffer)
	{
	}

private:
	Array<T, size> m_Buffer;
};

template<size_t size>
class LocalString : public FTLocalString<char, size>{};

template<size_t size>
class LocalWString : public FTLocalString<wchar_t, size>{};

}

#endif
