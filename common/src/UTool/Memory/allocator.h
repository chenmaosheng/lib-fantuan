#ifndef _H_ALLOCATOR
#define _H_ALLOCATOR

#include "../thread/scoped_locker.h"

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

		enum { OBJECT_OFFSET = FIELD_OFFSET(Object, data), };

		class DefaultAllocator
		{
		public:
			static void*		allocate(size_t bytes);
			static void			deallocate(void* ptr, size_t = 0);
			static void*		reallocate(void* ptr, size_t, size_t new_size);
		};

		class FTAllocator
		{
			friend class AllocatorAutoLocker;
			class AllocatorAutoLocker
			{
			public:
				AllocatorAutoLocker()
				{
					FTAllocator::m_Locker.Lock();
				}

				~AllocatorAutoLocker()
				{
					FTAllocator::m_Locker.Unlock();
				}
			};

			enum { ALIGNMENT = 8, MAX_BYTES = 256, NUM_LIST = 32, };

			static size_t		round_up(size_t bytes) 
			{ 
				return (((bytes) + (size_t) ALIGNMENT-1) & ~((size_t) ALIGNMENT - 1)); 
			}

			static size_t		freelist_index(size_t bytes)
			{
				return (((bytes) + (size_t)ALIGNMENT-1)/(size_t)ALIGNMENT - 1);
			}

			static uint8*		refill(size_t bytes);
			static uint8*		chunk_alloc(size_t bytes, int& num_object);

		public:
			static void*		allocate(size_t bytes);
			static void			deallocate(void* ptr);
			static void*		reallocate(void* ptr, size_t old_size, size_t new_size);

		private:
			static Object* volatile
				m_pFreeList[NUM_LIST];
			static uint8*		m_pStartFree[NUM_LIST];
			static uint8*		m_pEndFree[NUM_LIST];
			static size_t		m_iTotalSize[NUM_LIST];

			static ScopedLocker	m_Locker;
		};
	}

	typedef Allocator::FTAllocator FT_Alloc;
}

#endif
