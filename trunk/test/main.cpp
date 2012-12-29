#include "base.h"
#include <cstdio>
#include "array.h"

using namespace Fantuan;

struct AA
{
	int a;
};

int main(int argc, char* argv[])
{
	argc = argc;
	argv = argv;
	uint64 time = GET_TIME();
	
	//uint8 i = NumberPower(100);
	//printf("%d\n", i);

	Array<int, 100> m;
	m[0] = 10;
	m[1] = 100;

	//printf("%d\n", m[1]);

	ScopedPtr<AA> x(new AA);
	x->a = 1;

	uint64 end = GET_TIME();
	printf("time: %lluus\n", (end - time) / 1000);

	getchar();

	return 0;
}

