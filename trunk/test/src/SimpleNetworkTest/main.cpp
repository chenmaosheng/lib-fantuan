#include "simple_network.h"
#include "simple_connection.h"
#include "simple_worker.h"
#include <vector>

#define MAX_CONNECTION 10000

int main(int argc, char* argv[])
{
	EasyLog::Instance()->Init(EasyLog::LOG_WARNING_LEVEL);
	SimpleNetwork::Init();
	SimpleConnection* pConnection = new SimpleConnection;
	pConnection->Connect("127.0.0.1", 9001);
	
	SimpleWorker* pWorker = new SimpleWorker(pConnection);
	while (true)
	{
		Sleep(100);
	}

	
	return 0;
}