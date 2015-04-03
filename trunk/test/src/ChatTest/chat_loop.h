#ifndef _H_CHAT_LOOP
#define _H_CHAT_LOOP

#include "easy_baseloop.h"

class ChatLoop : public EasyBaseLoop
{
public:
	virtual EasySession* _CreateSession() { return new EasySession; }
};

#endif
