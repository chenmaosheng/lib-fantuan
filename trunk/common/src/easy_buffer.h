#ifndef _H_EASY_BUFFER
#define _H_EASY_BUFFER

#include "base.h"

class EasyBuffer
{
public:
	EasyBuffer(uint32 iMaxSize);
	~EasyBuffer();

	// write to buffer
	bool	Push(const void* pItem, uint32 iLength);
	uint32	Pop(void* pItem, uint32 iLength);
	inline	uint32	Size() const
	{
		return m_iSize;
	}

private:
	void*			m_pData;
	void* volatile	m_pTail;
	uint32			m_iMaxSize;
	uint32			m_iSize;
};
#endif
