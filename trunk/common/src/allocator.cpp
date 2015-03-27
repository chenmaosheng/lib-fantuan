#include "allocator.h"

namespace Fantuan
{
	namespace Allocator
	{
		void* DefaultAllocator::allocate(size_t bytes)
		{
			Object* pObject = (Object*)malloc(bytes);
			pObject->obj_size = bytes;
			return pObject->data;
		}

		void DefaultAllocator::deallocate(void* ptr, size_t)
		{
			free(ptr);
		}

		Object* FTAllocator::m_pFreeList[NUM_LIST] = {0};
		size_t	FTAllocator::m_iTotalSize[NUM_LIST] = {0};
		MutexLocker FTAllocator::m_Locker;
		
		void* FTAllocator::allocate(size_t bytes)
		{
			bytes += OBJECT_OFFSET;
			if (bytes > MAX_BYTES)
			{
				return DefaultAllocator::allocate(bytes);
			}

			Object** freeList = m_pFreeList + _index(bytes);

			AllocatorAutoLocker locker;
			Object* pObject = *freeList;
			if (!pObject)
			{
				return _refill(_round_up(bytes));
			}

			*freeList = pObject->free_list_link;
			return pObject->data;
		}

		void FTAllocator::deallocate(void* ptr)
		{
			Object* pObject = (Object*)((uint8*)ptr - OBJECT_OFFSET);
			if (pObject->obj_size > MAX_BYTES)
			{
				DefaultAllocator::deallocate(pObject);
				return;
			}

			Object** freeList = m_pFreeList + _index(pObject->obj_size);

			AllocatorAutoLocker locker;
			pObject->free_list_link = *freeList;
			*freeList = pObject;
		}

		uint8* FTAllocator::_refill(size_t bytes)
		{
			int32 nobject = 20;
			uint8* chunk = _chunk_alloc(bytes, nobject);
			
			Object* pObject = (Object*)chunk;
			pObject->obj_size = bytes;

			if (nobject != 1)
			{
				Object* curr_obj, *next_obj;
				Object** freeList = m_pFreeList + _index(bytes);
				*freeList = next_obj = (Object*)((uint8*)pObject + bytes);

				for (int i = 0; i < nobject - 2; ++i)
				{
					next_obj->obj_size = bytes;
					curr_obj = next_obj;
					next_obj = (Object*)((uint8*)next_obj + bytes);
					curr_obj->free_list_link = next_obj;
				}

				next_obj->obj_size = bytes;
				next_obj->free_list_link = 0;
			}

			return pObject->data;
		}

		uint8* FTAllocator::_chunk_alloc(size_t bytes, int32& nobject)
		{
			size_t index = _index(bytes);
			int32 total_bytes = bytes * nobject;
			int32 new_bytes = total_bytes * 2 + ROUND_UP((m_iTotalSize[index] >> 4), bytes);
			uint8* chunk = (uint8*)malloc(new_bytes);

			m_iTotalSize[index] += new_bytes;
			nobject = new_bytes / bytes;
			return chunk;
		}
	}
}

