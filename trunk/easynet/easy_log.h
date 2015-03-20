#ifndef _H_EASY_LOG
#define _H_EASY_LOG

#include <stdbool.h>
#include <sys/types.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_MAX_SIZE 65535

enum
{
	LOG_DEBUG_LEVEL,
	LOG_STATE_LEVEL,
	LOG_WARNING_LEVEL,
	LOG_ERROR_LEVEL,
};

struct _EasyBuf;
typedef struct _EasyLog
{
	int level_;
	pthread_t thread_;
	pthread_mutex_t lock_;
	struct _EasyBuf* easy_buf_;
	char buffer_[BUFFER_MAX_SIZE];
}EasyLog;

void StartEasyLog(int iLogLevel);
void StopEasyLog();
bool PushEasyLog(int iLogLevel, char* strFormat, ...);
int GetEasyLogLevel();

#define LOG_DBG(Expression, ...)				\
	if (GetEasyLogLevel() <= LOG_DEBUG_LEVEL)	\
		PushEasyLog(LOG_DEBUG_LEVEL, "[LINE:%-4d]" Expression, __LINE__);

#ifdef __cplusplus
}
#endif

#endif

