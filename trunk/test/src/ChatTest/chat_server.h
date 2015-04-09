#ifndef _H_CHAT_SERVER
#define _H_CHAT_SERVER

#include "easy_baseserver.h"

class ChatServer : public EasyBaseServer
{
public:
	virtual EasySession* _CreateSession() { return new EasySession; }
};

#endif
