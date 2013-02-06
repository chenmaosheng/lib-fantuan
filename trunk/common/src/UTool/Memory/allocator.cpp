#include "allocator.h"

namespace Fantuan
{

	namespace Allocator
	{

		void*	DefaultAllocator::allocate(size_t bytes)
		{
			Object* pObject = (Object*)malloc(bytes);
			pObject->obj_size = bytes;
			return pObject->data;
		}

		void	DefaultAllocator::deallocate(void* ptr, size_t)
		{
			Object* pObject = (Object*)((uint8*)ptr - OBJECT_OFFSET);
			free(pObject);
		}

		void*	DefaultAllocator::reallocate(void* ptr, size_t, size_t new_size)
		{
			return realloc(ptr, new_size);
		}

		uint8* FTAllocator::m_pStartFree[FTAllocator::NUM_LIST] = 
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		uint8* FTAllocator::m_pEndFree[FTAllocator::NUM_LIST] = 
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		size_t FTAllocator::m_iTotalSize[FTAllocator::NUM_LIST] = 
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		Object* volatile 
			FTAllocator::m_pFreeList[FTAllocator::NUM_LIST] = 
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		ScopedLocker FTAllocator::m_Locker;

		void* FTAllocator::allocate(size_t bytes)
		{
			bytes += OBJECT_OFFSET;

			if (bytes > MAX_BYTES)
			{
				return DefaultAllocator::allocate(bytes);
			}

			Object* volatile* freelist = m_pFreeList + freelist_index(bytes);

			AllocatorAutoLocker locker;
			Object* pObject = *freelist;
			if (!pObject)
			{
				// empty freelist
				size_t newBytes = round_up(bytes);
				return refill(newBytes);
			}

			// set new freelist
			*freelist = pObject->free_list_link;
			return pObject->data;
		}

		void FTAllocator::deallocate(void* ptr)
		{
			Object* pObject = (Object*)((uint8*)ptr - OBJECT_OFFSET);
			if (pObject->obj_size > MAX_BYTES)
			{
				DefaultAllocator::deallocate(ptr);
				return;
			}

			Object* volatile* freelist = m_pFreeList + freelist_index(pObject->obj_size);

			AllocatorAutoLocker locker;
			pObject->free_list_link = *freelist;
			*freelist = pObject;
		}

		uint8* FTAllocator::chunk_alloc(size_t bytes, int &_nobject)
		{
			uint8* chunkMem;
			size_t index = freelist_index(bytes);
			size_t total_bytes = bytes * _nobject;
			size_t left_bytes = m_pEndFree[index] - m_pStartFree[index];

			if (left_bytes >= total_bytes)
			{
				chunkMem = m_pStartFree[index];
				m_pStartFree[index] += total_bytes;
				return chunkMem;
			}

			// can't provide all of them
			if (left_bytes >= bytes)
			{
				_nobject = (int)(left_bytes / bytes);
				total_bytes = bytes * _nobject;
				chunkMem = m_pStartFree[index];
				m_pStartFree[index] += total_bytes;
				return chunkMem;
			}

			// ready to enlarge
			size_t new_bytes = total_bytes * 2 + round_up((m_iTotalSize[index] >> 4));
			void* pMem = malloc(new_bytes);

			m_iTotalSize[index] += new_bytes;
			m_pStartFree[index] = (uint8*)pMem;
			m_pEndFree[index] = m_pStartFree[index] + new_bytes;
			return chunk_alloc(bytes, _nobject);
		}

		uint8* FTAllocator::refill(size_t bytes)
		{
			int _nobject = 20;
			uint8* chunk = chunk_alloc(bytes, _nobject);
			Object* volatile* freelist;
			Object* pObject;
			Object* curr_obj;
			Object* next_obj;

			pObject = (Object*)chunk;
			pObject->obj_size = bytes;

			if (_nobject == 1)
			{
				return pObject->data;
			}

			freelist = m_pFreeList + freelist_index(bytes);
			*freelist = next_obj = (Object*)(chunk + bytes);
			next_obj->obj_size = bytes;
			for (int i = 1; ; i++)
			{
				curr_obj = next_obj;
				next_obj = (Object*)((uint8*)next_obj + bytes);
				next_obj->obj_size = bytes;
				if (_nobject - 1 == i)
				{
					curr_obj->obj_size = bytes;
					curr_obj->free_list_link = 0;
					break;
				}

				curr_obj->free_list_link = next_obj;
			}

			return pObject->data;
		}

	}

}