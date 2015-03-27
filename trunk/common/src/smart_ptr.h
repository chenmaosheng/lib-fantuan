#ifndef _H_SMART_PTR
#define _H_SMART_PTR

#include "util.h"
#include "assert.h"

namespace Fantuan
{

template<typename T>
class ScopedPtr
{
public:
	typedef T element_type;
	explicit ScopedPtr(T* pPointer = 0) : m_pPointer(pPointer)
	{
	}

	~ScopedPtr()
	{
		SAFE_DELETE(m_pPointer);
	}

	T&	operator*() const
	{
		FT_ASSERT( m_pPointer != 0 );
		return *m_pPointer;
	}

	T*	operator->() const
	{
		FT_ASSERT( m_pPointer != 0 );
		return m_pPointer;
	}

	T*	get() const
	{
		return m_pPointer;
	}

	operator bool() const
	{
		return m_pPointer != 0;
	}

private:
	ScopedPtr(const ScopedPtr&);
	ScopedPtr& operator=(const ScopedPtr&);

	void operator==(const ScopedPtr&) const;
	void operator!=(const ScopedPtr&) const;

private:
	T*	m_pPointer;
};

}

#endif
