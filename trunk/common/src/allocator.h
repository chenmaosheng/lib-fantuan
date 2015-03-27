#ifndef _H_ALLOCATOR
#define _H_ALLOCATOR

#include "mutex_locker.h"
#include "util.h"
#include "type.h"

namespace Fantuan
{
	namespace Allocator
	{
		struct Object
		{
			uint32			obj_size;
			union
			{
				Object*		free_list_link;
				uint8		data[1];
			};
		};

		enum { OBJECT_OFFSET = offsetof(Object, data), };
		enum { ALIGNMENT = 8, MAX_BYTES = 256, NUM_LIST = MAX_BYTES / ALIGNMENT,};

		class DefaultAllocator
		{
		public:
			static void*	allocate(size_t bytes);
			static void		deallocate(void* ptr, size_t = 0);
		};

		class FTAllocator
		{
			friend class AllocatorAutoLocker;
			class AllocatorAutoLocker
			{
			public:
				AllocatorAutoLocker()
				{
					FTAllocator::m_Locker.AcquireLock();
				}

				~AllocatorAutoLocker()
				{
					FTAllocator::m_Locker.ReleaseLock();
				}
			};

		public:
			static void*	allocate(size_t bytes);
			static void		deallocate(void* ptr);

		private:
			static size_t	_round_up(size_t bytes)
			{
				return ROUND_UP(bytes, ALIGNMENT);
			}
			
			static size_t	_index(size_t bytes)
			{
				return (bytes + ALIGNMENT - 1) / ALIGNMENT - 1;
			}

			static uint8*	_refill(size_t bytes);
			static uint8*	_chunk_alloc(size_t bytes, int32& num_object);

		private:
			static Object*	m_pFreeList[NUM_LIST];
			static size_t	m_iTotalSize[NUM_LIST];

			static MutexLocker	m_Locker;
		};
	}

	typedef Allocator::FTAllocator FT_Alloc;
}

#endif // _H_ALLOCATOR

