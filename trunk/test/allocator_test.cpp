#include "base.h"
#include "allocator_test.h"
#include "allocator.h"
#include "time_scope.h"
#include <vector>
#include <stdlib.h>

using namespace Fantuan;

#define ARRAY_TEST_TIME	1000000
#define ARRAY_SIZE		10000

struct AA
{
	AA(){
		data = new char[50];
	}

	AA(const AA& a)
	{
		data = new char[50];
		memcpy(data, a.data, 50);
	}

	const AA& operator=(const AA& a)
	{
		memcpy(data, a.data, 50);

		return *this;
	}

	~AA()
	{
		delete data;
	}

	char* data;
};

struct BB
{
	char* data;

	BB()
	{
		data = new char[rand() % 1000 + 1];
	}

	~BB()
	{
		delete data;
	}
};

void AllocatorTest1()
{
	printf("allocator and deallocator AA 1m times \n");
	AA* aa[ARRAY_SIZE] = {0};
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME / ARRAY_SIZE; ++i)
		{
			for (int j = 0; j < ARRAY_SIZE; ++j)
			{
				aa[j] = new AA;
			}

			for (int j = 0; j < ARRAY_SIZE; ++j)
			{
				delete aa[j];
			}
		}
	}
}

void AllocatorTest2()
{
	printf("vector<AA*, 1000000> allocator test\n");
	AA a;
	std::vector<AA*> aa;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
		{
			aa.push_back(&a);
		}

		aa.clear();
	}
}

void AllocatorTest3()
{
	printf("random allocator test 1m times\n");
	BB* aa[ARRAY_SIZE] = {0};
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME / ARRAY_SIZE; ++i)
		{
			for (int j = 0; j < ARRAY_SIZE; ++j)
			{
				aa[j] = new BB;
			}

			for (int j = 0; j < ARRAY_SIZE; ++j)
			{
				delete aa[j];
			}
		}
	}
}