#ifndef _H_ALLOCATOR
#define _H_ALLOCATOR

#include "util.h"

namespace Fantuan
{


template<size_t N>
class DefaultAllocator
{
	enum { ALIGNMENT = 8, MAX_BYTES = 256, NUM_LIST = 32, };

	struct Object
	{
		struct Object*	free_list_link;
		size_t			obj_size;
	};

	static size_t		freelist_index(size_t bytes);
	static void*		refill(size_t bytes);
	static char*		chunk_alloc(size_t bytes, int& num_object);

public:
	static void*		allocate(size_t bytes);
	static void			deallocate(void* ptr, size_t bytes);
	static void			deallocate(void* ptr);
	static void*		reallocate(void* ptr, size_t old_size, size_t new_size);

private:
	static Object* volatile
						m_pFreeList[NUM_LIST];
	static char*		m_pStartFree;
	static char*		m_pEndFree;
	static size_t		m_iTotalSize;
};

template<size_t N>
char* DefaultAllocator<N>::m_pStartFree = 0;

template<size_t N>
char* DefaultAllocator<N>::m_pEndFree = 0;

template<size_t N>
size_t DefaultAllocator<N>::m_iTotalSize = 0;

template<size_t N>
typename DefaultAllocator<N>::Object* volatile 
DefaultAllocator<N>::m_pFreeList[
	DefaultAllocator<N>::NUM_LIST
] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

typedef DefaultAllocator<0> FT_Alloc;

template<size_t N>
size_t DefaultAllocator<N>::freelist_index(size_t bytes)
{
	return (bytes + ALIGNMENT - 1) / ALIGNMENT - 1;
}

template<size_t N>
void* DefaultAllocator<N>::allocate(size_t n)
{
	void* ret = 0;
	if (n > MAX_BYTES)
	{
		ret = malloc(n);
	}
	else
	{
		Object* volatile* freelist = m_pFreeList + freelist_index(n);
		
		//AutoLocker locker(&m_Mutex);
		Object* result = *freelist;
		if (!result)
		{
			ret = refill(ROUND_UP(n, ALIGNMENT));
		}
		else
		{
			*freelist = result->free_list_link;
			ret = result;
		}
	}

	return ret;
}

template<size_t N>
void DefaultAllocator<N>::deallocate(void* ptr, size_t n)
{
	if (n > MAX_BYTES)
	{
		free(ptr);
	}
	else
	{
		Object* volatile* freelist = m_pFreeList + freelist_index(n);
		Object* temp = (Object*)ptr;

		//AutoLocker locker(&m_Mutex);
		temp->free_list_link = *freelist;
		*freelist = temp;
	}
}

template<size_t N>
void DefaultAllocator<N>::deallocate(void* ptr)
{
	Object* temp = (Object*)ptr;
	if (temp->obj_size > MAX_BYTES)
	{
		free(ptr);
	}
	else
	{
		Object* volatile* freelist = m_pFreeList + freelist_index(temp->obj_size);

		//AutoLocker locker(&m_Mutex);
		temp->free_list_link = *freelist;
		*freelist = temp;
	}
}

template<size_t N>
void* DefaultAllocator<N>::reallocate(void* ptr, size_t old_size, size_t new_size)
{
	void* result;
	size_t copy_size;

	if (old_size > MAX_BYTES && new_size > MAX_BYTES)
	{
		return realloc(ptr, new_size);
	}

	if (ROUND_UP(old_size, ALIGNMENT) == ROUND_UP(new_size, ALIGNMENT))
	{
		return ptr;
	}

	result = allocate(new_size);
	copy_size = new_size > old_size ? old_size : new_size;
	memcpy(result, ptr, copy_size);
	deallocate(ptr, old_size);
	return result;
}

template<size_t N>
char* DefaultAllocator<N>::chunk_alloc(size_t size, int &_nobject)
{
	char* result;
	size_t total_bytes = size * _nobject;
	size_t bytes_left = m_pEndFree - m_pStartFree;
	
	if (bytes_left >= total_bytes)
	{
		result = m_pStartFree;
		m_pStartFree += total_bytes;
		return result;
	}
	else if (bytes_left >= size)
	{
		_nobject = (int)(bytes_left/size);
		total_bytes = size * _nobject;
		result = m_pStartFree;
		m_pStartFree += total_bytes;
		return result;
	}

	size_t new_bytes = total_bytes * 4;// + ROUND_UP((m_iTotalSize >> 8), ALIGNMENT);

	if (bytes_left > 0)
	{
		Object* volatile* freelist = m_pFreeList + freelist_index(bytes_left);
		((Object*)m_pStartFree)->free_list_link = *freelist;
		*freelist = (Object*)m_pStartFree;
	}

	printf("start malloc, new_bytes: %d\n", new_bytes);
	try
	{
		m_pStartFree = (char*)malloc(new_bytes);
	}
	catch (...)
	{
		printf("bad malloc\n");	
	}
	
	printf("end malloc\n");
	m_iTotalSize += new_bytes;
	printf("total: %d, new: %d, align: %d\n", m_iTotalSize, new_bytes, new_bytes - total_bytes * 2);
	m_pEndFree = m_pStartFree + new_bytes;
	return chunk_alloc(size, _nobject);
}

template<size_t N>
void* DefaultAllocator<N>::refill(size_t size)
{
	int _nobject = 20;
	char* chunk = chunk_alloc(size, _nobject);
	Object* volatile* freelist;
	Object* result;
	Object* curr_obj;
	Object* next_obj;
	int i;

	if (_nobject == 1)
	{
		return chunk;
	}
	freelist = m_pFreeList + freelist_index(size);
	
	result = (Object*)chunk;
	result->obj_size = size;
	*freelist = next_obj = (Object*)(chunk + size);
	next_obj->obj_size = size;
	for (i = 1; ; i++)
	{
		curr_obj = next_obj;
		next_obj = (Object*)((char*)next_obj + size);
		next_obj->obj_size = size;
		if (_nobject - 1 == i)
		{
			curr_obj->obj_size = size;
			curr_obj->free_list_link = 0;
			break;
		}

		curr_obj->free_list_link = next_obj;
	}

	return result;
}

template<typename T>
class allocator
{
	typedef FT_Alloc _Alloc;

public:
	allocator() {}
	allocator(const allocator&) {}
	template<typename T>
	allocator(const allocator<T>&) {}
	~allocator() {}

	T* allocate(size_t n, const void* = 0)
	{
		return n != 0 ? static_cast<T*>(
			_Alloc::allocate(n * sizeof(T))) : 0;
	}

	void deallocate(T* ptr, size_t n)
	{
		_Alloc::deallocate(ptr, n * sizeof(T));
	}

	void deallocate(T* ptr)
	{
		_Alloc::deallocate(ptr);
	}
};

}

#endif
