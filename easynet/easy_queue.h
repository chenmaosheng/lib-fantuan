#ifndef _H_EASY_QUEUE
#define _H_EASY_QUEUE

#include "easy_locker.h"
#include <queue>

template<typename T>
struct EasyQueue
{
	void Push(T* pItem)
	{
		EasyAutoLocker autoLocker(&locker_);
		container_.push(pItem);
	}
	
	T* Pop()
	{
		T* pItem = NULL;
		EasyAutoLocker autoLocker(&locker_);
		if (container_.empty())
		{
			pItem = new T();
		}
		else
		{
			pItem = container_.front();
			container_.pop();
		}
		return pItem;
	}

	std::queue<T*> container_; 
	EasyLocker locker_;
};

#endif
