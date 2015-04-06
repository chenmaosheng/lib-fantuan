#include "simple_worker.h"
#include "simple_connection.h"

SimpleWorker::SimpleWorker(SimpleConnection* pConnection)
{
	thread_ = (HANDLE)_beginthreadex(NULL, 0, &SimpleWorker::SimpleThread, pConnection, 0, NULL);
}

SimpleWorker::~SimpleWorker()
{
	CloseHandle(thread_);
}

uint32 WINAPI SimpleWorker::SimpleThread(PVOID pParam)
{
	SimpleConnection* pConnection = (SimpleConnection*)pParam;
	int32 ret = 0;
	while (true)
	{
		fd_set readFDs, exceptFDs;
		FD_ZERO(&readFDs);
		FD_ZERO(&exceptFDs);
		FD_SET(pConnection->socket_, &readFDs);
		
		timeval val;
		val.tv_sec = 0;
		val.tv_usec = 10000;
		ret = select(0, &readFDs, NULL, &exceptFDs, &val);
		if (ret < 0)
		{
			LOG_ERR(_T("connection failure, error=%d"), GetLastError());
			pConnection->Close();
			return -1;
		}
		else if (ret > 0)
		{
			if (FD_ISSET(pConnection->socket_, &readFDs))
			{
				pConnection->RecvData();
				//FD_CLR(pConnection->socket_, &readFDs);
			}
			else if (FD_ISSET(pConnection->socket_, &exceptFDs))
			{
				pConnection->Close();
				LOG_ERR(_T("connection lost"));
				return -1;
			}
		}
	}

	return 0;
}