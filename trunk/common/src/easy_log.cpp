#include "easy_log.h"
#include "easy_buffer.h"

EasyLog::EasyLog() :
	m_iLogLevel(LOG_DEBUG_LEVEL),
	m_OutputEvent(NULL)
{
	memset(m_strBuffer, 0, sizeof(m_strBuffer));
	m_pEasyBuffer = new EasyBuffer(BUFFER_MAX_SIZE);
	m_OutputEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

EasyLog::~EasyLog()
{
	SAFE_DELETE(m_pEasyBuffer);
	CloseHandle(m_OutputEvent);
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

bool EasyLog::Push(int32 iLogLevel, wchar_t* strFormat, ...)
{
	wchar_t Buffer[BUFFER_MAX_SIZE] = {0};
	wchar_t RealBuffer[BUFFER_MAX_SIZE] = {0};
	va_list Args;

	va_start(Args, strFormat);
	int32 iLength = vswprintf(NULL, 0, strFormat, Args) + 1;
	va_end(Args);
	va_start(Args, strFormat);
	vswprintf(Buffer, iLength, strFormat, Args);
	va_end(Args);
	time_t now = time(NULL);
	tm* currentTime = localtime(&now);
	swprintf(RealBuffer, BUFFER_MAX_SIZE, _T("[%02d.%02d.%02d %02d:%02d:%02d][%s]%s\n"), 
		currentTime->tm_year+1900, currentTime->tm_mon+1, currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec,
		_Level2String(iLogLevel), Buffer);

	std::lock_guard<std::mutex> guard(m_Mutex);
	SetEvent(m_OutputEvent);
	return m_pEasyBuffer->Push(RealBuffer, (int32)wcslen(RealBuffer) * sizeof(wchar_t));
}

uint32 __stdcall EasyLog::_LogOutput(void* pParam)
{
	EasyLog* pLog = (EasyLog*)pParam;
	while (true)
	{
		pLog->_Tick();
	}

	return 0;
}

wchar_t* EasyLog::_Level2String(int32 iLogLevel)
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
	DWORD ret = ::WaitForSingleObject(m_OutputEvent, INFINITE);
	if (ret == WAIT_OBJECT_0)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);
		uint16 iSize = (uint16)m_pEasyBuffer->Size();
		uint16 iLength = 0;
		memset(m_strBuffer, 0, sizeof(m_strBuffer));
		iLength = m_pEasyBuffer->Pop(m_strBuffer, iSize);
		if (iLength)
		{
			ResetEvent(m_OutputEvent);
			_Output(m_strBuffer, iLength);
		}
	}
}

void EasyLog::_Output(wchar_t* strBuffer, uint16 iCount)
{
	wprintf(_T("%s"), strBuffer);
}
