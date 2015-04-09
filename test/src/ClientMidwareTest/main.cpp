#include "easy_baseclient.h"

int main(int argc, char* argv[])
{
	EasyBaseClient* pClient= new EasyBaseClient();
	pClient->Login("127.0.0.1", 9001);

	while (true)
	{
		easy_sleep(10);
	}
}