#include "base.h"
#include <cstdio>
#include "array.h"
#include <vector>

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

inline void object_swap(AA& lhs, const AA& val)
{
	lhs.TransferOwnership(val);
}

namespace Fantuan
{

template<>
inline void object_swap(std::string& lhs, const std::string& val)
{
	lhs = val;
}

}

int AA::count = 0;

void Func(ArrayBase<std::string>& data)
{
	printf("%s\n", data[1].c_str());
}

int main(int argc, char* argv[])
{
	std::string a("HelloHelloHelloHelloHelloHelloHelloHello");
	//DArray<std::string, 1000> m;
	std::vector<std::string> m;
	
	uint64 time = GET_TIME();
	for (int i = 0; i < 1000000; ++i){
		m.push_back(a);
	}

	uint64 end = GET_TIME();
	printf("time: %lluus\n", (end - time) / 1000);

	//Func(m);

	DArray<int, 1000> n;
	//std::vector<int> n;
	time = GET_TIME();
	for (int i = 0; i < 1000000; ++i)
		n.push_back(100);

	end = GET_TIME();
	printf("time: %lluus\n", (end - time) / 1000);
	getchar();

	return 0;
}

