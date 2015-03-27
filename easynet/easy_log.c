#include "easy_log.h"
#include "easy_buf.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

EasyLog easy_log_;

void* OutputEasyLog(void* ptr)
{
	int iCurrSize = 0;
	int iLength = 0;
	
	while (true)
	{
		pthread_mutex_lock(&easy_log_.lock_);
		printf("start to run\n");
		iCurrSize = (int)easy_log_.easy_buf_->curr_size_;
		printf("currsize: %d\n", iCurrSize);
		memset(easy_log_.buffer_, 0, sizeof(easy_log_.buffer_));
		iLength = PopFromEasyBuf(easy_log_.easy_buf_, easy_log_.buffer_, iCurrSize);
		printf("length: %d\n", iLength);
		if (iLength != 0)
		{
			printf("%s", easy_log_.buffer_);
		}
		pthread_mutex_unlock(&easy_log_.lock_);
	}
}

char* Level2String(int iLogLevel)
{
	switch(iLogLevel)
	{
		case LOG_DEBUG_LEVEL:
			return "DEBUG";
		case LOG_WARNING_LEVEL:
			return "WARNING";
		case LOG_ERROR_LEVEL:
			return "ERROR";
		case LOG_STATE_LEVEL:
			return "STATE";
		default:
			return "";
	}
};

void StartEasyLog(int iLogLevel)
{
	easy_log_.easy_buf_ = CreateEasyBuf(BUFFER_MAX_SIZE);
	printf("createeasybuf\n");
	memset(easy_log_.buffer_, 0, sizeof(easy_log_.buffer_));
	easy_log_.level_ = iLogLevel;
	pthread_mutex_init(&easy_log_.lock_, NULL);
	pthread_create(&easy_log_.thread_, NULL, OutputEasyLog, NULL);
	printf("start thread\n");
}

void StopEasyLog()
{
	pthread_mutex_destroy(&easy_log_.lock_);
	free(easy_log_.easy_buf_);
}

int GetEasyLogLevel()
{
	return easy_log_.level_;
}

bool PushEasyLog(int iLogLevel, char* strFormat, ...)
{
	char buffer[BUFFER_MAX_SIZE];
	char realBuffer[BUFFER_MAX_SIZE];
	int iLength = 0;
	
	va_list Args;
	va_start(Args, strFormat);
	iLength = vsnprintf(buffer, 0, strFormat, Args) + 1;
	va_end(Args);
	va_start(Args, strFormat);
	vsnprintf(buffer, iLength, strFormat, Args);
	va_end(Args);
	snprintf(realBuffer, BUFFER_MAX_SIZE, "[%s]%s\r\n", Level2String(iLogLevel), buffer);
	pthread_mutex_lock(&easy_log_.lock_);
	PushToEasyBuf(easy_log_.easy_buf_, realBuffer, (int)strlen(realBuffer));
	pthread_mutex_unlock(&easy_log_.lock_);
	return true;
}


#ifdef __cplusplus
}
#endif

