#include "base.h"
#include "allocator.h"
#include "array_test.h"
#include "allocator_test.h"
#include <vector>
#include "dy_array.h"
#include <tchar.h>
#include "psapi.h"
#include <DbgHelp.h>

using namespace Fantuan;

static TCHAR szModuleName[MAX_PATH] = {0};
static TCHAR szDumpPath[MAX_PATH] = {0};
static DWORD dwProcessId = 0;
static HANDLE hProcess = NULL;

void CreateMiniDump(_EXCEPTION_POINTERS* ExceptionInfo);

LONG WINAPI UnhandledExceptionFilter2(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	CreateMiniDump(ExceptionInfo);
	return(EXCEPTION_EXECUTE_HANDLER);
}
void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
	CreateMiniDump(NULL);
	exit(0);
} 
void UnHandledPurecallHandler(void)
{
	CreateMiniDump(NULL);
	exit(0);	
}

void InitDump()
{	
	DWORD dwRet = 0;
	TCHAR* p = NULL;

	dwRet = GetModuleBaseName(GetCurrentProcess(), NULL, szModuleName, MAX_PATH);
	if (dwRet == 0)
	{
		_stprintf_s(szModuleName, MAX_PATH, _T("TServer"));
	}
	if(GetEnvironmentVariable(_T("TYRoot"), szDumpPath, MAX_PATH) == 0)
	{
		int a= GetLastError();
		GetModuleFileName(NULL, szDumpPath, MAX_PATH);
		p = _tcsstr(szModuleName, _T("."));
		if (p) *p = _T('\0');
		p = _tcsstr(szDumpPath, szModuleName);
		if (p) *p = _T('\0');
		_tcscat(szDumpPath, szModuleName);
	}
	else
	{
		_tcscat(szDumpPath, _T("\\"));
		_tcscat(szDumpPath, szModuleName);
	}

	_stprintf_s(szDumpPath, MAX_PATH, _T("%s_%llu.dmp"), szDumpPath, time(NULL));

	hProcess = GetCurrentProcess();
	dwProcessId = GetCurrentProcessId();

	SetUnhandledExceptionFilter(UnhandledExceptionFilter2);
	_set_invalid_parameter_handler(InvalidParameterHandler);
	_set_purecall_handler(UnHandledPurecallHandler);
}



void CreateMiniDump(_EXCEPTION_POINTERS* ExceptionInfo)
{
	BOOL bRet = 0;
	DWORD dwErr = 0;
	HANDLE hDmp = 0;	
	MINIDUMP_EXCEPTION_INFORMATION eInfo;

	eInfo.ThreadId = GetCurrentThreadId();
	eInfo.ExceptionPointers = ExceptionInfo;
	eInfo.ClientPointers = FALSE;

	hDmp = CreateFile(szDumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH, NULL);
	if(hDmp==INVALID_HANDLE_VALUE) 
	{
		::MessageBox(NULL, _T("ERROR"), _T("Fail to create dump file"), MB_OK);
		return;
	}
	bRet = MiniDumpWriteDump(hProcess, dwProcessId, hDmp, MiniDumpWithFullMemory, ExceptionInfo?&eInfo:NULL, NULL, NULL);
	if (!bRet)
	{
		bRet = MiniDumpWriteDump(hProcess, dwProcessId, hDmp, MiniDumpNormal, ExceptionInfo?&eInfo:NULL, NULL, NULL);
		if (!bRet)
		{
			dwErr = GetLastError();
			WriteFile(hDmp, &dwErr, 4, NULL, NULL);
			CloseHandle(hDmp);
			::MessageBox(NULL, _T("create dump fail, call me 909"), _T("ERROR"), MB_OK);
			while(true) Sleep(1);
			return;
		}
	}
	CloseHandle(hDmp);
}

void* operator new(size_t n)
{
	return FT_Alloc::allocate(n);
}

void* operator new[](size_t n)
{
	return ::operator new(n);
}

void operator delete(void* ptr)
{
	FT_Alloc::deallocate(ptr);
}

void operator delete[](void* ptr)
{
	operator delete(ptr);
}

int main(int argc, char* argv[])
{
	argc = argc; argv = argv;
	InitDump();
	
	ArrayTest1();
	printf("\n\n");
	ArrayTest2();
	printf("\n\n");
	ArrayTest3();
	printf("\n\n");
	DyArrayTest1();
	printf("\n\n");
	DyArrayTest2();
	printf("\n\n");
	DyArrayTest3();

	AllocatorTest1();
	printf("\n\n");
	AllocatorTest2();
	printf("\n\n");
	AllocatorTest3();
	printf("\n\n");

	getchar();

	return 0;
}

