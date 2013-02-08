#ifndef _H_ARG
#define _H_ARG

#include "type_list.h"

namespace Fantuan
{

class Arg
{
public:

private:
	void*	m_pData;
};

#define ArgListInit(FuncName, Format, Handler)	\
	void FuncName(Format format) { Handler(format, TYPE_LIST_0()); }	\
	void FuncName(Format format, Arg1 arg1) { Handler(format, TYPE_LIST_1(arg1)); }	\
	void FuncName(Format format, Arg1 arg1, Arg2 arg2) { Handler(format, TYPE_LIST_2(arg1, arg2)); }
}

#endif
