#ifndef _H_ACCURATE_TIME
#define _H_ACCURATE_TIME

#include "common.h"
#include "singleton.h"

namespace Fantuan
{

class AccurateTime : public Singleton<AccurateTime>
{
public:
	AccurateTime()
	{
#ifdef WIN32
		QueryPerformanceFrequency((_LARGE_INTEGER*)&m_iPerformanceFrequency);
#endif
	}

	inline uint64 GetTime()
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

}

#define GET_TIME()	Fantuan::AccurateTime::Instance()->GetTime()

#endif
