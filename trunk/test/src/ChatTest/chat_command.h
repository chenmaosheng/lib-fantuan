#ifndef _H_CHAT_COMMAND
#define _H_CHAT_COMMAND

#include "easy_command.h"

enum
{
	COMMAND_ONPINGREQ = COMMAND_APPSTART + 1,
	COMMAND_ONCHATREQ = COMMAND_APPSTART + 2,
};

#endif
