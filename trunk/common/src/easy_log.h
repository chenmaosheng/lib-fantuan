#ifndef _H_EASY_LOG
#define _H_EASY_LOG

#include "base.h"
#include "singleton.h"

class EasyLogDevice;
class EasyBuffer;
class EasyLog : public Singleton<EasyLog>
{
public:
	enum { LOG_DEBUG_LEVEL, LOG_WARNING_LEVEL, LOG_ERROR_LEVEL, LOG_STATE_LEVEL, };
	enum { LOG_DEVICE_CONSOLE, LOG_DEVICE_DEBUG, LOG_DEVICE_FILE, };
	enum { BUFFER_MAX_SIZE = 65535, };

	EasyLog();
	~EasyLog();

	void	Init(int32 iLogLevel = LOG_DEBUG_LEVEL);
	void	Destroy();
	void	AddLogDevice(EasyLogDevice*);
	bool	Push(int32 iLogLevel, const wchar_t* strFormat, ...);
	int32	GetLogLevel() const
	{
		return m_iLogLevel;
	}	

	const wchar_t*	_Level2String(int32 iLogLevel);
	static uint32	_LogOutput(void*);
	void			_Tick();
	void			_Output(wchar_t* strBuffer, uint16 iCount);

private:
	wchar_t			m_strBuffer[BUFFER_MAX_SIZE];
	EasyBuffer*		m_pEasyBuffer;
	int32			m_iLogLevel;
	std::mutex		m_Mutex;
	std::condition_variable	m_OutputEvent;
	std::thread*	m_pThread;
};

#define LOG_DBG(Expression, ...)										\
	if (EasyLog::Instance()->GetLogLevel() <= EasyLog::LOG_DEBUG_LEVEL)					\
	EasyLog::Instance()->Push(EasyLog::LOG_DEBUG_LEVEL, _T("[%u:%-30s][LINE:%-4u] ") Expression, EASY_THREADID, FUNCTION_NAME, __LINE__, ##__VA_ARGS__);

#define LOG_WAR(Expression, ...)										\
	if (EasyLog::Instance()->GetLogLevel() <= EasyLog::LOG_WARNING_LEVEL)				\
	EasyLog::Instance()->Push(EasyLog::LOG_WARNING_LEVEL, _T("[%u:%-30s][LINE:%-4u] ") Expression, EASY_THREADID, FUNCTION_NAME, __LINE__, ##__VA_ARGS__);

#define LOG_ERR(Expression, ...)										\
	if (EasyLog::Instance()->GetLogLevel() <= EasyLog::LOG_ERROR_LEVEL)					\
	EasyLog::Instance()->Push(EasyLog::LOG_ERROR_LEVEL, _T("[%u:%-30s][LINE:%-4u] ") Expression, EASY_THREADID, FUNCTION_NAME, __LINE__, ##__VA_ARGS__);

#define LOG_STT(Expression, ...)										\
	if (EasyLog::Instance()->GetLogLevel() <= EasyLog::LOG_STATE_LEVEL)				\
	EasyLog::Instance()->Push(EasyLog::LOG_STATE_LEVEL, _T("[%u:%-30s][LINE:%-4u] ") Expression, EASY_THREADID, FUNCTION_NAME, __LINE__, ##__VA_ARGS__);

#endif
