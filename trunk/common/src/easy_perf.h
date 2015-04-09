#ifndef _H_EASY_PERF
#define _H_EASY_PERF

#include "base.h"

struct FunctionProfiler
{
	FunctionProfiler(const TCHAR* strFuncName);
	void Print();

	const TCHAR*	m_strFuncName;
	uint32			m_iCallCount;
	uint64			m_iTimeCost;
};

class EasyPerf
{

};
#endif
