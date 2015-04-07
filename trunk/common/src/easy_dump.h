#ifndef _H_EASY_DUMP
#define _H_EASY_DUMP

#include "base.h"

#ifdef WIN32

class EasyDump
{
public:
	static void Init(TCHAR* strPath=NULL);

private:
	static void _Create(_EXCEPTION_POINTERS* pExceptionPointers);
	static LONG WINAPI _UnhandledExceptionFilter( __in struct _EXCEPTION_POINTERS *ExceptionInfo );
	static void _InvalidParameterHandler(const TCHAR* strExpression, const TCHAR* strFunction, const TCHAR* strFileName, uint32 iLine, uintptr_t iPreserved);
	static void _UnHandledPurecallHandler();

private:
	static TCHAR m_strDumpPath[MAX_PATH];
	static HANDLE m_hProcess;
	static DWORD m_dwProcessId;
};

#endif

#endif
