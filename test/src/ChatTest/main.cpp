#include "chat_server.h"

int main(int argc, char* argv[])
{
	ChatServer* pServer = new ChatServer();
	pServer->Initialize(0, 9001);

	while (true)
	{
		easy_sleep(10);
	}
}