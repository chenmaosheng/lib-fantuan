#ifndef _H_CHAT_SESSION
#define _H_CHAT_SESSION

#include "easy_session.h"

class ChatSession : public EasySession
{
public:
	void OnChatReq(uint32 iLen, char* message);
};
#endif
