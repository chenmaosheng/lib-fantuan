#ifndef _H_ARG
#define _H_ARG

#include "dy_array.h"

#define ARGLIST_MAX		64

namespace Fantuan
{


class Arg
{
public:
	template<typename T>
	Arg(const T& value)
	{
		m_pData = &value;
	}

private:
	void*	m_pData;
};

class ArgList : public DyArray<Arg, ARGLIST_MAX>
{
public:
	typedef DyArray<Arg, ARGLIST_MAX> baseClass;

	ArgList& operator<<(const Arg& arg)
	{
		baseClass::push_back(arg);
		return *this;
	}
};

#endif // _H_ARG
