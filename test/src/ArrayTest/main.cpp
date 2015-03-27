#include "common.h"
#include "allocator.h"
#include "array_test.h"
#include "allocator_test.h"
#include "string_test.h"

using namespace Fantuan;

void* operator new(size_t n)
{
	return FT_Alloc::allocate(n);
}

void* operator new[](size_t n)
{
	return operator new(n);
}

void operator delete(void* ptr)
{
	FT_Alloc::deallocate(ptr);
}

void operator delete[](void* ptr)
{
	operator delete(ptr);
}

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

	StringTest1();
	printf("\n\n");
	
	getchar();

	return 0;
}

