#ifndef _H_CHAT_SERVER
#define _H_CHAT_SERVER

#include "easy_baseserver.h"
#include "chat_loop.h"

class ChatServer : public EasyBaseServer
{
public:
	virtual EasyBaseLoop* _CreateServerLoop() { return new ChatLoop; }
};

#endif
