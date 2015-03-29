#include "easy_command.h"

EasyCommandOnData::EasyCommandOnData()
{
	m_iLen = 0;
	m_pData = NULL;
}

EasyCommandOnData::~EasyCommandOnData()
{
	_aligned_free(m_pData);
}

bool EasyCommandOnData::CopyData(uint16 iLen, const char *pData)
{
	m_pData = (char*)_aligned_malloc(iLen, MEMORY_ALLOCATION_ALIGNMENT);
	EASY_ASSERT(m_pData);
	if (m_pData)
	{
		m_iLen = iLen;
		memcpy(m_pData, pData, iLen);
		return true;
	}

	return false;
}

EasyCommandSendData::EasyCommandSendData()
{
	m_iSessionId = 0;
	m_iTypeId = 0;
	m_iLen = 0;
	m_pData = NULL;
}

EasyCommandSendData::~EasyCommandSendData()
{
	_aligned_free(m_pData);
}

bool EasyCommandSendData::CopyData(uint16 iLen, const char* pData)
{
	m_pData = (char*)_aligned_malloc(iLen, MEMORY_ALLOCATION_ALIGNMENT);
	EASY_ASSERT(m_pData);
	if (m_pData)
	{
		m_iLen = iLen;
		memcpy(m_pData, pData, iLen);
		return true;
	}

	return false;
}

