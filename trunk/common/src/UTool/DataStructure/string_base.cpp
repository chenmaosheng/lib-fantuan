#include "base.h"
#include "string_base.h"

namespace Fantuan
{

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
