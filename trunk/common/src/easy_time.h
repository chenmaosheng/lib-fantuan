#ifndef _H_EASY_TIME
#define _H_EASY_TIME

#include "base.h"
#include "singleton.h"

#define GET_CURR_TIME	EasyTime::Instance()->GetCurrTime()

class EasyTime : public Singleton<EasyTime>
{
public:
	EasyTime()
	{
#ifdef WIN32
		QueryPerformanceFrequency((_LARGE_INTEGER*)&m_iPerformanceFrequency);
#endif
	}

	// get nanoseconds
	inline uint64 GetCurrTime()
	{
		uint64 time = 0;
#ifdef WIN32
		QueryPerformanceCounter((LARGE_INTEGER*)&time);
		time = time * 1000000000 / m_iPerformanceFrequency;
#else
		timespec timeSpec;
		clock_gettime(CLOCK_MONOTONIC, &timeSpec);
		time = (uint64)timeSpec.tv_sec * 1000000000 + timeSpec.tv_nsec;
#endif
		return time;
	}

private:
	uint64	m_iPerformanceFrequency;
};

#endif
