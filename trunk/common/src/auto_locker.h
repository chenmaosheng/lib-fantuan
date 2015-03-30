#ifndef _H_AUTO_LOCKER
#define _H_AUTO_LOCKER

#include "common.h"

class AutoLocker
{
public:
	AutoLocker(Mutex* pMutex) : m_pMutex(pMutex)
	{
		LockMutex(m_pMutex);
	}

	~AutoLocker()
	{
		UnlockMutex(m_pMutex);
	}

private:
	Mutex*	m_pMutex;
};

#endif

