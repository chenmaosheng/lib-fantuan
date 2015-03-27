#ifndef _H_SINGLETON
#define _H_SINGLETON

namespace Fantuan
{

template<typename T>
class Singleton
{
public:
	// global access pointer to the singleton
	static T*	Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new T();
		}

		return m_pInstance;
	}

private:
	static T*	m_pInstance;
};

template<typename T>
T* Singleton<T>::m_pInstance = 0;

}

#endif

