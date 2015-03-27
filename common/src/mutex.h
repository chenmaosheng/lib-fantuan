#ifndef _H_MUTEX
#define _H_MUTEX

#include "common.h"

namespace Fantuan
{

#ifdef WIN32
	typedef CRITICAL_SECTION ft_mutex;
#else
	typedef pthread_mutex_t ft_mutex;
#endif

inline void ft_mutex_init(ft_mutex* pMutex)
{
#ifdef WIN32
	InitializeCriticalSectionAndSpinCount(pMutex, 4000);
#else
	pthread_mutex_init(pMutex, NULL);
#endif
}

inline void ft_mutex_destroy(ft_mutex* pMutex)
{
#ifdef WIN32
	DeleteCriticalSection(pMutex);
#else
	pthread_mutex_destroy(pMutex);
#endif
}

inline void ft_mutex_lock(ft_mutex* pMutex)
{
#ifdef WIN32
	EnterCriticalSection(pMutex);
#else
	pthread_mutex_lock(pMutex);
#endif
}

inline void ft_mutex_unlock(ft_mutex* pMutex)
{
#ifdef WIN32
	LeaveCriticalSection(pMutex);
#else
	pthread_mutex_unlock(pMutex);
#endif
}

}
#endif

