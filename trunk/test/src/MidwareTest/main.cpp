#include "easy_baseserver.h"

int main(int argc, char* argv[])
{
	EasyBaseServer* pServer = new EasyBaseServer();
	pServer->Initialize(0, 9001);

	while (true)
	{
		Sleep(10000);
	}
}