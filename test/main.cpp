#include "base.h"
#include "allocator.h"
#include "array_test.h"
#include <vector>
#include "dy_array.h"

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



struct BB
{
	char x[50];

	~BB(){}
};

int main(int argc, char* argv[])
{
	argc = argc; argv = argv;
	uint64 time = GET_TIME();
	for (int i = 0; i < 10000000; ++i)
	{
		//printf("%d ", i);
		new BB;
	}
	uint64 end = GET_TIME();
	printf("time: %lluus\n", (end - time) / 1000);

	std::string hello("HelloHelloHelloHelloHelloHelloHelloHello");
	BB b;

	//DyArray<BB> m;
	////std::vector<BB> m;
	//uint64 time = GET_TIME();
	//for (int i = 0; i < 100000; ++i)
	//	m.push_back(b);

	//uint64 end = GET_TIME();
	//printf("time: %lluus\n", (end - time) / 1000);

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
	DyArrayTest3();*/

	getchar();

	return 0;
}

