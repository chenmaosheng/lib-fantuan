#ifndef _H_ALLOCATOR
#define _H_ALLOCATOR

#include "util.h"

namespace Fantuan
{

class DefaultAllocator
{
	enum { ALIGNMENT = 8, MAX_BYTES = 256, NUM_LIST = 32, };

	struct Object
	{
		struct Object*	free_list_link;
		size_t			obj_size;
		char			data[1];
	};

	enum { OBJECT_OFFSET = sizeof(Object*) + sizeof(size_t), };

	static size_t		freelist_index(size_t bytes)
	{
		return (bytes + ALIGNMENT - 1) / ALIGNMENT - 1;
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
};

char* DefaultAllocator::m_pStartFree[DefaultAllocator::NUM_LIST] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

char* DefaultAllocator::m_pEndFree[DefaultAllocator::NUM_LIST] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

size_t DefaultAllocator::m_iTotalSize[DefaultAllocator::NUM_LIST] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

DefaultAllocator::Object* volatile 
DefaultAllocator::m_pFreeList[DefaultAllocator::NUM_LIST] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

typedef DefaultAllocator FT_Alloc;

void* DefaultAllocator::allocate(size_t bytes)
{
	if (bytes > MAX_BYTES)
	{
		return malloc(bytes);
	}

	bytes += OBJECT_OFFSET;
		
	Object* volatile* freelist = m_pFreeList + freelist_index(bytes);
		
	//AutoLocker locker(&m_Mutex);
	Object* pObject = *freelist;
	if (!pObject)
	{
		// empty freelist
		return refill(ROUND_UP(bytes, ALIGNMENT));
	}
	
	// set new freelist
	*freelist = pObject->free_list_link;
	return pObject->data;
}

void DefaultAllocator::deallocate(void* ptr, size_t bytes)
{
	if (bytes > MAX_BYTES)
	{
		free(ptr);
		return;
	}

	Object* volatile* freelist = m_pFreeList + freelist_index(bytes);
	Object* pObject = (Object*)ptr;

	//AutoLocker locker(&m_Mutex);
	pObject->free_list_link = *freelist;
	*freelist = pObject;
}

void DefaultAllocator::deallocate(void* ptr)
{
	Object* pObject = (Object*)((char*)ptr - FIELD_OFFSET(Object, data));
	deallocate(ptr, pObject->obj_size);
}

char* DefaultAllocator::chunk_alloc(size_t bytes, int &_nobject)
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
	size_t new_bytes = total_bytes * 2 + ROUND_UP((m_iTotalSize[index] >> 4), ALIGNMENT);
	m_pStartFree[index] = (char*)malloc(new_bytes);
	
	m_iTotalSize[index] += new_bytes;
	m_pEndFree[index] = m_pStartFree[index] + new_bytes;
	return chunk_alloc(bytes, _nobject);
}

char* DefaultAllocator::refill(size_t bytes)
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

#endif
