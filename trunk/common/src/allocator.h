#ifndef _H_ALLOCATOR
#define _H_ALLOCATOR

#include "base.h"
#include "util.h"

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
	enum { ALIGNMENT = 8, MAX_BYTES = 512, NUM_LIST = MAX_BYTES / ALIGNMENT,};

	class DefaultAllocator
	{
	public:
		static void*	allocate(uint32 bytes);
		static void		deallocate(void* ptr, uint32 = 0);
	};

	class FTAllocator
	{
	public:
		static void*	allocate(uint32 bytes);
		static void		deallocate(void* ptr);

	private:
		static uint32	_round_up(uint32 bytes)
		{
			return ROUND_UP(bytes, ALIGNMENT);
		}

		static uint32	_index(uint32 bytes)
		{
			return (bytes + ALIGNMENT - 1) / ALIGNMENT - 1;
		}

		static uint8*	_refill(uint32 bytes);
		static uint8*	_chunk_alloc(uint32 bytes, int32& num_object);

	private:
		static Object*	m_pFreeList[NUM_LIST];
		static uint32	m_iTotalSize[NUM_LIST];

		static std::mutex	m_Locker;
	};
}

typedef Allocator::FTAllocator FT_Alloc;

#endif // _H_ALLOCATOR

