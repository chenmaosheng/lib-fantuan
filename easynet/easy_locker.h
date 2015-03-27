#ifndef _H_EASY_LOCKER
#define _H_EASY_LOCKER

#include "common.h"
#include <pthread.h>

struct EasyLocker
{
	EasyLocker()
	{
		pthread_mutex_init(mutex_, NULL);
	}

	~EasyLocker()
	{
		pthread_mutex_destroy(mutex_);
	}

	void AcquireLock()
	{
		pthread_mutex_lock(mutex_);
	}

	void ReleaseLocker()
	{
		pthread_mutex_unlock(mutex_);
	}

	pthread_mutex_t mutex_;
};

struct EasyAutoLocker
{
	EasyAutoLocker(EasyLocker* pLocker) : locker_(pLocker)
	{
		pLocker->AcquireLock();
	}

	~EasyAutoLocker()
	{
		locker_->ReleaseLocker();
	}

	EasyLocker* locker_;
};

#endif
