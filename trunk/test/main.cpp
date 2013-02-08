#include "base.h"
#include "allocator.h"
#include "array_test.h"
#include "allocator_test.h"
#include <vector>
#include "dy_array.h"
#include "array.h"
#include "local_string.h"
#include "dy_string.h"

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

	LocalString<32> aa("I love you, yanxi");
	printf("len=%d, data=%s\n", aa.strlen(), aa.c_str());
	LocalString<32> bb;
	bb = "Hello, World";
	printf("len=%d, data=%s\n", bb.strlen(), bb.c_str());

	String cc("I love you, too");
	printf("len=%d, data=%s\n", cc.strlen(), cc.c_str());
	String dd;
	dd = "Hello, World, too";
	printf("len=%d, data=%s\n", dd.strlen(), dd.c_str());
	getchar();

	return 0;
}

