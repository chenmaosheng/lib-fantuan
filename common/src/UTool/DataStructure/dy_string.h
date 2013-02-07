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
	FTString() : StringBase<T>(&m_Buffer)
	{
	}
private:
	DyArray<T> m_Buffer;
};

typedef FTString<char, STRING_DEFAULT_SIZE> String;
typedef FTString<wchar_t, STRING_DEFAULT_SIZE> WString;

}

#endif
