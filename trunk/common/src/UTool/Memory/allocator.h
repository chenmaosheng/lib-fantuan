#ifndef _H_ALLOCATOR
#define _H_ALLOCATOR

#include "util.h"

namespace Fantuan
{

	namespace Allocator
	{

	class AllocatorLock
	{
	public:
		AllocatorLock()
		{
			::InitializeCriticalSection(&m_Mutex);
		}

		~AllocatorLock()
		{
			::DeleteCriticalSection(&m_Mutex);
		}

		void Lock()
		{
			::EnterCriticalSection(&m_Mutex);
		}

		void Unlock()
		{
			::LeaveCriticalSection(&m_Mutex);
		}

	private:
		CRITICAL_SECTION	m_Mutex;
	};

	struct Object
	{
		size_t			obj_size;
		union
		{
			Object*		free_list_link;
			char		data[1];
		};
	};

	enum { OBJECT_OFFSET = FIELD_OFFSET(Object, data), };

	class DefaultAllocator
	{
	public:
		static void*		allocate(size_t bytes)
		{
			Object* pObject = (Object*)malloc(bytes + OBJECT_OFFSET);
			return pObject->data;
		}

		static void			deallocate(void* ptr, size_t = 0)
		{
			Object* pObject = (Object*)((char*)ptr - OBJECT_OFFSET);
			free(pObject);
		}

		static void*		reallocate(void* ptr, size_t, size_t new_size)
		{
			return realloc(ptr, new_size);
		}
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

		static char*		refill(size_t bytes);
		static char*		chunk_alloc(size_t bytes, int& num_object);

	public:
		static void*		allocate(size_t bytes);
		static void			deallocate(void* ptr, size_t bytes);
		static void			deallocate(void* ptr);
		static void*		reallocate(void* ptr, size_t old_size, size_t new_size);

	private:
		static Object* volatile
			m_pFreeList[NUM_LIST];
		static char*		m_pStartFree[NUM_LIST];
		static char*		m_pEndFree[NUM_LIST];
		static size_t		m_iTotalSize[NUM_LIST];

		static AllocatorLock	m_Locker;
	};

	char* FTAllocator::m_pStartFree[FTAllocator::NUM_LIST] = 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	char* FTAllocator::m_pEndFree[FTAllocator::NUM_LIST] = 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	size_t FTAllocator::m_iTotalSize[FTAllocator::NUM_LIST] = 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	Object* volatile 
		FTAllocator::m_pFreeList[FTAllocator::NUM_LIST] = 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	AllocatorLock FTAllocator::m_Locker;

	typedef FTAllocator FT_Alloc;

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

	void FTAllocator::deallocate(void* ptr, size_t bytes)
	{
		if (bytes > MAX_BYTES)
		{
			DefaultAllocator::deallocate(ptr, bytes);
			return;
		}

		Object* volatile* freelist = m_pFreeList + freelist_index(bytes);
		Object* pObject = (Object*)ptr;

		AllocatorAutoLocker locker;
		pObject->free_list_link = *freelist;
		*freelist = pObject;
	}

	void FTAllocator::deallocate(void* ptr)
	{
		Object* pObject = (Object*)((char*)ptr - OBJECT_OFFSET);
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

	char* FTAllocator::chunk_alloc(size_t bytes, int &_nobject)
	{
		char* chunkMem;
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
		//printf("start malloc\n");
		void* pMem = malloc(new_bytes);
		//printf("end malloc, pMem=0x%08x\n", pMem);

		m_iTotalSize[index] += new_bytes;
		m_pStartFree[index] = (char*)pMem;
		m_pEndFree[index] = m_pStartFree[index] + new_bytes;
		return chunk_alloc(bytes, _nobject);
	}

	char* FTAllocator::refill(size_t bytes)
	{
		int _nobject = 20;
		char* chunk = chunk_alloc(bytes, _nobject);
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
			next_obj = (Object*)((char*)next_obj + bytes);
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

#endif
