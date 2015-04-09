#include "easy_baseserver.h"

class EasyServer : public EasyBaseServer
{
public:
	virtual EasySession* _CreateSession() { return new EasySession; }
};

int main(int argc, char* argv[])
{
	EasyServer* pServer = new EasyServer();
	pServer->Initialize(0, 9001);

	while (true)
	{
		easy_sleep(10);
	}
}