#include "base.h"
#include "allocator.h"
#include "array_test.h"
#include "allocator_test.h"
#include <vector>
#include "dy_array.h"
#include "array.h"

using namespace Fantuan;

void* operator new(size_t n)
{
	return FT_Alloc::allocate(n);
}

void* operator new[](size_t n)
{
	return ::operator new(n);
}

void operator delete(void* ptr)
{
	FT_Alloc::deallocate(ptr);
}

void operator delete[](void* ptr)
{
	operator delete(ptr);
}

template<size_t size>
class SimpleString
{
public:
	const static size_t string_size = size;

	SimpleString(const char* strString)
	{
		int32 i = 0;
		while (strString[i] != '\0' && i < string_size)
		{
			m_Data[i] = strString[i];
			++i;
		}
		m_Data[i] = '\0';
	}

	char* c_str() const
	{
		return (char*)m_Data.data();
	}

private:
	Array<char, size>	m_Data;
};

int main(int argc, char* argv[])
{
	argc = argc; argv = argv;
	
	/*ArrayTest1();
	printf("\n\n");
	ArrayTest2();
	printf("\n\n");
	ArrayTest3();
	printf("\n\n");
	DyArrayTest1();
	printf("\n\n");
	DyArrayTest2();
	printf("\n\n");
	DyArrayTest3();

	AllocatorTest1();
	printf("\n\n");
	AllocatorTest2();
	printf("\n\n");
	AllocatorTest3();
	printf("\n\n");*/

	SimpleString<256> str("I love you");
	printf("%s\n", str.c_str());

	getchar();

	return 0;
}

