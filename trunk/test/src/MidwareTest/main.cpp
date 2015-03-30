#include "easy_baseserver.h"
#include "easy_baseloop.h"

class EasyLoop : public EasyBaseLoop
{
public:
	virtual EasySession* _CreateSession() { return new EasySession; }
};

class EasyServer : public EasyBaseServer
{
public:
	virtual EasyBaseLoop* _CreateServerLoop() { return new EasyLoop; }
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