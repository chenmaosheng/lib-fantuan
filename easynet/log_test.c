#include "easy_log.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	char name[] = "Maosheng";
	StartEasyLog(LOG_DEBUG_LEVEL);
	LOG_DBG("Hello");
	return 0;
}

