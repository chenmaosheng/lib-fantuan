#ifndef _H_EASY_POOL
#define _H_EASY_POOL

#include "common.h"
#include <list>

template<typename T>
class EasyPool
{
public:
	EasyPool()
	{
	}

	~EasyPool()
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		for (std::list<T*>::iterator it = m_ObjectList.begin(); it != m_ObjectList.end(); ++it)
		{
			SAFE_DELETE(*it);
		}
	}

	T*	Allocate()
	{
		T* pObject = NULL;
		std::unique_lock<std::mutex> lock(m_Mutex);
		if (m_ObjectList.empty())
		{
			pObject = new T();
		}
		else
		{
			pObject = m_ObjectList.front();
			m_ObjectList.pop_front();
		}
		lock.unlock();
		return pObject;
	}

	void Free(T* pObject)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_ObjectList.push_back(pObject);
	}

private:
	std::list<T*>	m_ObjectList;
	std::mutex m_Mutex;
};
#endif
