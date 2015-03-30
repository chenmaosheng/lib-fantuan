#ifndef _H_MUTEX_LOCKER
#define _H_MUTEX_LOCKER

#include "mutex.h"

class MutexLocker
{
public:
	MutexLocker()
	{
		ft_mutex_init(&m_Mutex);
	}

	~MutexLocker()
	{
		ft_mutex_destroy(&m_Mutex);
	}

	void AcquireLock()
	{
		ft_mutex_lock(&m_Mutex);
	}

	void ReleaseLock()
	{
		ft_mutex_unlock(&m_Mutex);
	}

private:
	ft_mutex m_Mutex;
};

#endif

