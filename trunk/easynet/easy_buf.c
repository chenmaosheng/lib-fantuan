#include "easy_buf.h"
#include <string.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MEMORY_ALLOCATION_ALIGNMENT 8

EasyBuf* CreateEasyBuf(size_t max_size)
{
	EasyBuf* pBuf = (EasyBuf*)memalign(MEMORY_ALLOCATION_ALIGNMENT, sizeof(EasyBuf));
	pBuf->data_ = memalign(MEMORY_ALLOCATION_ALIGNMENT, max_size);
	memset(pBuf->data_, 0, max_size);
	pBuf->tail_ = pBuf->data_;
	pBuf->size_ = max_size;
	pBuf->curr_size_ = 0;
	return pBuf;
}

void	DestroyEasyBuf(EasyBuf* pBuf)
{
	free(pBuf->data_);
	free(pBuf);
}

bool	PushToEasyBuf(EasyBuf* pBuf, const void* pItem, size_t iLength)
{
	if (pBuf->curr_size_ + iLength >= pBuf->size_)
	{
		return false;
	}

	memcpy(pBuf->tail_, pItem, iLength);
	pBuf->tail_ = (char*)pBuf->tail_ + iLength;
	pBuf->curr_size_ += iLength;
	return true;
}

size_t	PopFromEasyBuf(EasyBuf* pBuf, void* pItem, size_t iLength)
{
	if (!pItem)
	{
		return 0;
	}

	if (pBuf->curr_size_ < iLength)
	{
		iLength = pBuf->curr_size_;
	}

	pBuf->tail_ = (char*)pBuf->tail_ - iLength;
	memcpy(pItem, pBuf->tail_, iLength);
	pBuf->curr_size_ -= iLength;

	return iLength;
}

#ifdef __cplusplus
}
#endif

