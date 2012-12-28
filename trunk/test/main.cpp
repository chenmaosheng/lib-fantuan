#include "util.h"
#include <cstdio>
#include "array.h"

using namespace Fantuan;

int main(int argc, char* argv[])
{
	argc = argc;
	argv = argv;
	uint8 i = NumberPower(100);
	printf("%d\n", i);

	Array<int, 100> m;
	m[0] = 10;
	m[1] = 100;

	printf("%d\n", m[1]);

	getchar();

	return 0;
}

