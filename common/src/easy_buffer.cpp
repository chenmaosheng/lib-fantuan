#include "easy_buffer.h"

EasyBuffer::EasyBuffer(uint32 iMaxSize) : m_iMaxSize(iMaxSize), m_iSize(0)
{
	m_pData = _aligned_malloc(iMaxSize, MEMORY_ALLOCATION_ALIGNMENT);
	memset(m_pData, 0, iMaxSize);
	m_pTail = m_pData;
}

EasyBuffer::~EasyBuffer()
{
	_aligned_free(m_pData);
}

bool	EasyBuffer::Push(const void* pItem, uint32 iLength)
{
	if (m_iSize + iLength >= m_iMaxSize)
	{
		return false;
	}

	memcpy(m_pTail, pItem, iLength);
	m_pTail = (char*)m_pTail + iLength;
	m_iSize += iLength;

	return true;
}

uint32	EasyBuffer::Pop(void* pItem, uint32 iLength)
{
	if (pItem == nullptr)
	{
		return 0;
	}

	if (m_iSize < iLength)
	{
		iLength = m_iSize;
	}

	m_pTail = (char*)m_pTail - iLength;
	memcpy(pItem, m_pTail, iLength);
	m_iSize -= iLength;

	return iLength;
}
