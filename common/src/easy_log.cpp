#include "easy_log.h"
#include "easy_buffer.h"

EasyLog::EasyLog() :
	m_iLogLevel(LOG_DEBUG_LEVEL)
{
	memset(m_strBuffer, 0, sizeof(m_strBuffer));
	m_pEasyBuffer = new EasyBuffer(BUFFER_MAX_SIZE);
}

EasyLog::~EasyLog()
{
	SAFE_DELETE(m_pEasyBuffer);
}

void EasyLog::Init(int32 iLogLevel)
{
	m_iLogLevel = iLogLevel;
	m_pThread = new std::thread(&EasyLog::_LogOutput, this);
}

void EasyLog::Destroy()
{
	m_pThread->join();
}

bool EasyLog::Push(int32 iLogLevel, const wchar_t* strFormat, ...)
{
	wchar_t Buffer[BUFFER_MAX_SIZE] = {0};
	wchar_t RealBuffer[BUFFER_MAX_SIZE] = {0};
	va_list Args;

	va_start(Args, strFormat);
	int32 iLength = vswprintf(Buffer, BUFFER_MAX_SIZE, strFormat, Args) + 1;
	va_end(Args);
	time_t now = time(NULL);
	tm* currentTime = localtime(&now);
	swprintf(RealBuffer, BUFFER_MAX_SIZE, _T("[%02d.%02d.%02d %02d:%02d:%02d][%ls]%ls\n"), 
		currentTime->tm_year+1900, currentTime->tm_mon+1, currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec,
		_Level2String(iLogLevel), Buffer);

	std::unique_lock<std::mutex> lock(m_Mutex);
	m_OutputEvent.notify_one();
	return m_pEasyBuffer->Push(RealBuffer, (int32)wcslen(RealBuffer) * sizeof(wchar_t));
}

uint32 EasyLog::_LogOutput(void* pParam)
{
	EasyLog* pLog = (EasyLog*)pParam;
	while (true)
	{
		pLog->_Tick();
	}

	return 0;
}

const wchar_t* EasyLog::_Level2String(int32 iLogLevel)
{
	switch(iLogLevel)
	{
	case LOG_DEBUG_LEVEL:
		return _T("DEBUG");

	case LOG_WARNING_LEVEL:
		return _T("WARNI");

	case LOG_ERROR_LEVEL:
		return _T("ERROR");

	case LOG_STATE_LEVEL:
		return _T("STATE");

	default:
		return _T("");
	}
}

void EasyLog::_Tick()
{
	std::unique_lock<std::mutex> lock(m_Mutex);
	m_OutputEvent.wait(lock);
	uint16 iSize = (uint16)m_pEasyBuffer->Size();
	uint16 iLength = 0;
	memset(m_strBuffer, 0, sizeof(m_strBuffer));
	iLength = m_pEasyBuffer->Pop(m_strBuffer, iSize);
	if (iLength)
	{
		_Output(m_strBuffer, iLength);
	}
}

void EasyLog::_Output(wchar_t* strBuffer, uint16 iCount)
{
#ifdef WIN32
	wprintf(_T("%s"), strBuffer);
#else
	printf("%ls", strBuffer);
#endif
}
