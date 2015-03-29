#ifndef _H_EASY_STREAM
#define _H_EASY_STREAM

#include "common.h"

class Stream
{
public:
	Stream(uint32 iLength, const char* pBuf) : m_iDataLen(iLength), m_iDataIndex(0), m_pDataBuffer((char*)pBuf), m_bDelete(false)
	{
	}

	Stream() : m_iDataLen(0), m_bDelete(true)
	{
		m_pDataBuffer = (char*)_aligned_malloc(MAX_OUTPUT_BUFFER, MEMORY_ALLOCATION_ALIGNMENT);
	}

	~Stream()
	{
		if (m_bDelete)
		{
			_aligned_free(m_pDataBuffer);
		}
	}

	uint16 GetDataLength() const
	{
		return m_iDataLen;
	}

	const char* GetBuffer() const
	{
		return m_pDataBuffer;
	}

	template<typename T>
	bool Serialize(const T& value)
	{
		// serialize common type
		if (m_iDataLen + sizeof(T) <= sizeof(m_pDataBuffer))
		{
			memcpy(m_pDataBuffer+m_iDataLen, &value, sizeof(T));
			m_iDataLen += sizeof(T);
			return true;
		}

		return false;
	}

	template<typename T>
	bool Serialize(uint16 iCount, const T* array)
	{
		// serialize array type
		iCount *= sizeof(T);
		if (m_iDataLen + iCount <= sizeof(m_pDataBuffer))
		{
			memcpy(m_pDataBuffer + m_iDataLen, array, iCount);
			m_iDataLen += iCount;
			return true;
		}

		return false;
	}

	template<typename T>
	bool DeSerialize(T& value)
	{
		// serialize common type
		if (m_iDataLen - m_iDataIndex >= sizeof(T))
		{
			memcpy(&value, m_pDataBuffer + m_iDataIndex, sizeof(T));
			m_iDataIndex += sizeof(T);
			return true;
		}

		return false;
	}

	template<typename T>
	bool DeSerialize(uint16 iCount, T* array)
	{
		// serialize array type
		iCount *= sizeof(T);
		if (m_iDataLen - m_iDataIndex >= iCount)
		{
			memcpy(array, m_pDataBuffer + m_iDataIndex, iCount);
			m_iDataIndex += iCount;
			return true;
		}

		return false;
	}

private:
	uint16 m_iDataLen;
	uint16 m_iDataIndex;
	char* m_pDataBuffer;
	bool m_bDelete;
};

#endif
