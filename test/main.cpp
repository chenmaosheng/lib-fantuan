#include "base.h"
#include <cstdio>
#include "dy_array.h"
#include "array.h"
#include <vector>
#include <type_traits>

using namespace Fantuan;

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

struct BB
{
	int x;
};

inline void object_swap(AA& lhs, const AA& val)
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

int AA::count = 0;

void Func(ArrayBase<std::string>& data)
{
	printf("%s\n", data[100].c_str());
}

int main(int argc, char* argv[])
{
	argc = argc; argv = argv;

	std::string a("HelloHelloHelloHelloHelloHelloHelloHello");
	
	DyArray<std::string, 2> m;
	for (int i = 0; i < 4; ++i)
		m.push_back(a);
	
	DyArray< DyArray<std::string, 2> > n;
	n.push_back(m);
	
	uint64 time = GET_TIME();
	for (int i = 0; i < 100; ++i){
		m.push_back(a);
	}

	uint64 end = GET_TIME();
	printf("time: %lluus\n", (end - time) / 1000);

	getchar();

	return 0;
}

