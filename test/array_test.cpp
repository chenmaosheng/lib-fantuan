#include "array_test.h"
#include <cstdio>
#include "dy_array.h"
#include "array.h"
#include <vector>
#include "accurate_time.h"

#define ARRAY_TEST_TIME	1000000
#define ARRAY_SIZE		1000

using namespace Fantuan;

struct TimeScope
{
	TimeScope()
	{
		start = GET_TIME();
	}

	~TimeScope()
	{
		printf("cost time: %lluus\n", (GET_TIME() - start) / 1000);
	}

	uint64 start;
};

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
		++count;
	}

	mutable char* data;

	void TransferOwnership(const AA& val)
	{
		std::swap(data, val.data);
	}

	static int count;
};

int AA::count = 0;

inline void object_swap(AA& lhs, AA& val)
{
	lhs.TransferOwnership(val);
}

namespace Fantuan
{

template<>
inline void object_swap(std::string& lhs, std::string& rhs)
{
	lhs.swap(rhs);
}

}

void ArrayTest1()
{
	printf("Array<int, 1000> with 1M times access\n");
	Array<int, ARRAY_SIZE> a;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			a[i / 1000];
	}

	printf("DyArray<int, 1000> with 1M times access\n");
	DyArray<int, ARRAY_SIZE> b;
	{
		for (int i = 0; i < ARRAY_SIZE; ++i)
			b.push_back(100);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			b[i / 1000];

	}

	printf("int a[1000] with 1M times access\n");
	int c[ARRAY_SIZE];
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			c[i / 1000];
	}

	printf("vector<int, 1000> with 1M times access\n");
	std::vector<int> d;
	{
		d.resize(ARRAY_SIZE);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			a[i / 1000];
	}
}

void ArrayTest2()
{
	printf("Array<std::string, 1000> with 1M times access\n");
	Array<std::string, ARRAY_SIZE> a;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			a[i / 1000];
	}

	printf("DyArray<std::string, 1000> with 1M times access\n");
	std::string temp = "hello";
	DyArray<std::string, ARRAY_SIZE> b;
	{	
		for (int i = 0; i < ARRAY_SIZE; ++i)
			b.push_back(temp);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			b[i / 1000];
	}

	printf("std::string a[1000] with 1M times access\n");
	std::string c[ARRAY_SIZE];
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			c[i / 1000];
	}

	printf("vector<std::string, 1000> with 1M times access\n");
	std::vector<std::string> d;
	{
		d.resize(ARRAY_SIZE);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			d[i / 1000];
	}
}

void ArrayTest3()
{
	printf("Array<Array<int>, 1000> with 1M times access\n");
	Array< Array<int>, ARRAY_SIZE > a;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			a[i / 1000];
	}

	printf("Array<vector<int>, 1000> with 1M times access\n");
	Array< std::vector<int>, ARRAY_SIZE > b;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			b[i / 1000];
	}

	printf("DyArray<DyArray<int>, 1000> with 1M times access\n");
	DyArray<int> temp;
	DyArray< DyArray<int>, ARRAY_SIZE > c;
	{
		for (int i = 0; i < ARRAY_SIZE; ++i)
			c.push_back(temp);

		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			c[i / 1000];
	}

	printf("DyArray<vector<int>, 1000> with 1M times access\n");
	std::vector<int> temp2;
	DyArray< std::vector<int>, ARRAY_SIZE > d;
	{
		for (int i = 0; i < ARRAY_SIZE; ++i)
			d.push_back(temp2);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			d[i / 1000];
	}

	printf("vector<Array<int>, 1000> with 1M times access\n");
	std::vector< Array<int> > e;
	{
		e.resize(ARRAY_SIZE);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			e[i / 1000];
	}

	printf("vector<DyArray<int>, 1000> with 1M times access\n");
	std::vector< DyArray<int> > f;
	{
		f.resize(ARRAY_SIZE);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			f[i / 1000];
	}

	printf("vector<vector<int>, 1000> with 1M times access\n");
	std::vector< std::vector<int> > g;
	{	
		g.resize(ARRAY_SIZE);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			g[i / 1000];
	}

	printf("Array<int> a[1000] with 1M times access\n");
	Array<int> h[ARRAY_SIZE];
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			h[i / 1000];
	}

	printf("DyArray<int> a[1000] with 1M times access\n");
	DyArray<int> j[ARRAY_SIZE];
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			j[i / 1000];
	}

	printf("vector<int> a[1000] with 1M times access\n");
	std::vector<int> k[ARRAY_SIZE];
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			k[i / 1000];
	}
}

void DyArrayTest1()
{
	printf("DyArray<int> with 1M times push\n");
	DyArray<int> a;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			a.push_back(100);
	}

	printf("DyArray<int, 1000> with 1M times push\n");
	DyArray<int, 1000> b;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			b.push_back(100);
	}

	printf("vector<int> with 1M times push\n");
	std::vector<int> c;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			c.push_back(100);
	}

	printf("vector<int, 1000> with 1M times push\n");
	std::vector<int> d;
	{
		d.resize(1000);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			d.push_back(100);
	}
}

void DyArrayTest2()
{
	std::string temp("HelloHelloHelloHelloHelloHelloHelloHello");

	printf("DyArray<std::string> with 1M times push\n");
	DyArray<std::string> a;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			a.push_back(temp);
	}

	printf("DyArray<std::string, 1000> with 1M times push\n");
	DyArray<std::string, 1000> b;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			b.push_back(temp);
	}

	printf("vector<std::string> with 1M times push\n");
	std::vector<std::string> c;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			c.push_back(temp);
	}

	printf("vector<std::string, 1000> with 1M times push\n");
	std::vector<std::string> d;
	{
		d.resize(1000);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			d.push_back(temp);
	}
}

void DyArrayTest3()
{
	AA aa;

	printf("DyArray<AA> with 1M times push\n");
	DyArray<AA> a;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			a.push_back(aa);
	}

	printf("DyArray<AA, 1000> with 1M times push\n");
	DyArray<AA, 1000> b;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			b.push_back(aa);
	}

	printf("std::vector<AA> with 1M times push\n");
	std::vector<AA> c;
	{
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			c.push_back(aa);
	}

	printf("std::vector<AA, 1000> with 1M times push\n");
	std::vector<AA> d;
	{
		d.resize(1000);
		TimeScope t;
		for (int i = 0; i < ARRAY_TEST_TIME; ++i)
			d.push_back(aa);
	}
}

