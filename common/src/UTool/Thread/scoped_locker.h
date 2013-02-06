#ifndef _H_SCOPED_LOCKER
#define _H_SCOPED_LOCKER

#include "..\Base\base.h"

namespace Fantuan
{
	class ScopedLocker
	{
	public:
		ScopedLocker()
		{
			::InitializeCriticalSection(&m_Mutex);
		}

		~ScopedLocker()
		{
			::DeleteCriticalSection(&m_Mutex);
		}

		void Lock()
		{
			::EnterCriticalSection(&m_Mutex);
		}

		void Unlock()
		{
			::LeaveCriticalSection(&m_Mutex);
		}

	private:
		CRITICAL_SECTION	m_Mutex;
	};
}

#endif
