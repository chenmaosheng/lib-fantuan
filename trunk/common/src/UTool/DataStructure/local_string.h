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
	FTLocalString() : StringBase<T>(&m_Buffer){}

private:
	Array<T, size> m_Buffer;
};

template<size_t size>
class LocalString : public FTLocalString<char, size>
{
public:
	typedef StringBase<char> baseClass;

	LocalString() : FTLocalString<char, size>(){}

	LocalString(const char* pData) : FTLocalString<char, size>()
	{
		baseClass::assign(pData);
	}

	LocalString& operator=(const char* pData)
	{
		baseClass::assign(pData);
		return *this;
	}
};

template<size_t size>
class LocalWString : public FTLocalString<wchar_t, size>
{
	typedef StringBase<wchar_t> baseClass;

	LocalWString() : FTLocalString<wchar_t, size>(){}

	LocalWString(const wchar_t* pData) : FTLocalString<wchar_t, size>()
	{
		baseClass::assign(pData);
	}

	LocalWString& operator=(const wchar_t* pData)
	{
		baseClass::assign(pData);
		return *this;
	}
};

}

#endif
