#ifndef _H_DY_STRING
#define _H_DY_STRING

#include "string_base.h"
#include "dy_array.h"

#define STRING_DEFAULT_SIZE 64

namespace Fantuan
{

template<typename T, size_t size>
class FTString : public StringBase<T>
{
public:
	typedef StringBase<T> baseClass;

	FTString() : StringBase<T>(&m_Buffer)
	{
	}

	FTString(const T* pData) : StringBase<T>(&m_Buffer)
	{
		baseClass::assign(pData);
	}

	FTString& operator=(const T* pData)
	{
		baseClass::assign(pData);
		return *this;
	}

private:
	DyArray<T, size> m_Buffer;
};

typedef FTString<char, STRING_DEFAULT_SIZE> String;
typedef FTString<wchar_t, STRING_DEFAULT_SIZE> WString;

}

#endif
