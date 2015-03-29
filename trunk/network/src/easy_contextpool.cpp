#include "easy_contextpool.h"
#include "easy_context.h"

#ifdef WIN32

EasyContextPool::EasyContextPool(uint32 input_buffer_size, uint32 output_buffer_size)
{
	input_buffer_size_ = input_buffer_size;
	output_buffer_size_ = output_buffer_size;
	input_context_count_ = 0;
	output_context_count_ = 0;

	// initialize each Slist
	InitializeSListHead(&input_context_pool_);
	InitializeSListHead(&output_context_pool_);

	LOG_STT(_T("Initialize context pool success"));
}

EasyContextPool::~EasyContextPool()
{
	while (QueryDepthSList(&input_context_pool_) != input_context_count_)
	{
		Sleep(100);
	}

	while (QueryDepthSList(&input_context_pool_))
	{
		_aligned_free(InterlockedPopEntrySList(&input_context_pool_));
	}

	while (QueryDepthSList(&output_context_pool_) != output_context_count_)
	{
		Sleep(100);
	}

	while (QueryDepthSList(&output_context_pool_))
	{
		_aligned_free(InterlockedPopEntrySList(&output_context_pool_));
	}

	LOG_STT(_T("Destroy context pool success\n"));
}

EasyContext* EasyContextPool::PopInputContext()
{
	EasyContext* pContext = (EasyContext*)InterlockedPopEntrySList(&input_context_pool_);
	if (!pContext)
	{
		pContext = (EasyContext*)_aligned_malloc(sizeof(EasyContext)+input_buffer_size_, MEMORY_ALLOCATION_ALIGNMENT);
		_ASSERT(pContext);
		if (!pContext)
		{
			LOG_ERR(_T("Allocate context failed, err=%d"), GetLastError());
			return NULL;
		}

		ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
		pContext->operation_type_ = OPERATION_RECV;
		pContext->context_pool_ = this;
		pContext->wsabuf_.buf = (char*)pContext->buffer_;
		InterlockedIncrement((LONG*)&input_context_count_);
	}

	return pContext;
}

EasyContext* EasyContextPool::PopOutputContext()
{
	EasyContext* pContext = (EasyContext*)InterlockedPopEntrySList(&output_context_pool_);
	if (!pContext)
	{
		pContext = (EasyContext*)_aligned_malloc(sizeof(EasyContext)+output_buffer_size_, MEMORY_ALLOCATION_ALIGNMENT);
		_ASSERT(pContext);
		if (!pContext)
		{
			LOG_ERR(_T("Allocate context failed, err=%d"), GetLastError());
			return NULL;
		}

		ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
		pContext->operation_type_ = OPERATION_SEND;
		pContext->context_pool_ = this;
		pContext->wsabuf_.buf = (char*)pContext->buffer_;
		InterlockedIncrement((LONG*)&output_context_count_);
	}

	return pContext;
}

void EasyContextPool::PushInputContext(EasyContext* pContext)
{
	InterlockedPushEntrySList(&input_context_pool_, pContext);
}

void EasyContextPool::PushOutputContext(EasyContext* pContext)
{
	InterlockedPushEntrySList(&output_context_pool_, pContext);
}

char* EasyContextPool::PopOutputBuffer()
{
	EasyContext* pContext = PopOutputContext();
	_ASSERT(pContext);
	if (pContext)
	{
		return pContext->buffer_;
	}

	LOG_DBG(_T("Pop a buffer from pool failed, err=%d"), GetLastError());

	return NULL;
}

void EasyContextPool::PushOutputBuffer(char* buffer)
{
	PushOutputContext((EasyContext*)((char*)buffer - BUFOFFSET));
}

#endif
