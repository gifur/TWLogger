// LoggerTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TWLogger.h"
#include <cstdlib>

static unsigned int _stdcall threadFunc(void* param)
{
#if(TWINKLE_LOGGER_VERSION && TWINKLE_LOGGER_VERSION == 2)
	DWORD dwThreadID = GetCurrentThreadId();
	srand((unsigned int)time(0));
	tchar szMark[128] = {};
	_tprintf(L"Current thread: %d\n", dwThreadID);
	_stprintf_s(szMark, L"Thread-%d", dwThreadID);
	GetLoggerFactory()->GetLoggerProduct(szMark);
	GET_LOG_INSTANCE(szMark)->TraceDebug_f(L"I am here !");
	int randNum = 5 + 15 * rand()/(RAND_MAX + 1);
	Sleep(randNum * 1000);
	GET_LOG_INSTANCE(szMark)->TraceDebug_f(L"I am done !");
#endif
	return 0;
}

int Funct1()
{
	int nRet = 3;
	TRACE_FUNCTION(&nRet);

	nRet = 5;
	for(int i=0; i<100; ++i){
		//Sleep(50);
	}
	nRet = 6;
	return nRet;
}


DWORD Funct2()
{
	DWORD nRet = 3;
	TRACE_FUNCTION(&nRet);

	nRet = 5;
	for(int i=0; i<100; ++i) {
		//Sleep(50);
	}
	nRet = 10;
	return nRet;
}

void Funct3()
{
	LOG_FUNCTION();

	return;
}

void Funct4()
{
	//LOG_FUNCTION();
	HANDLE h10Thread[20] = {};
	for(int i=0; i<20; ++i) {
		h10Thread[i] = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0, threadFunc, NULL, 0, NULL));
	}
	DWORD dwRes = WaitForMultipleObjects(20, h10Thread, TRUE, 20000);
	_tprintf(L"dwRes: %d\n", dwRes);
	return;
}

void ShowLoggers()
{
#if(TWINKLE_LOGGER_VERSION && TWINKLE_LOGGER_VERSION == 2)
	LoggerList list;
	GetLoggerFactory()->InitializeLoggerList(list);
	LoggerList::const_iterator citer = list.cbegin();
	_tprintf(TLOG_TEXT("logger list(%d):\n"), list.size());
	while(citer != list.cend())
	{
#ifdef UNICODE
		_tprintf(TLOG_TEXT("name: %ws\n"), citer->GetName().c_str());
#else
		_tprintf(TLOG_TEXT("name: %s\n"), citer->GetName().c_str());
#endif
		citer++;
	}
	printf("\n");
#endif
}

int _tmain(int argc, _TCHAR* argv[])
{
	GET_LOGGER()->EnableConsole();

	LOG_FUNCTION();
#if(TWINKLE_LOGGER_VERSION && TWINKLE_LOGGER_VERSION == 2)

	GetLoggerFactory()->Test();
	GetLoggerFactory()->SaveAndCreateLogDirectory(L"C:\\LoggerTest\\123\\4\\5\\6\\7\\8\\9");
	GetLoggerFactory()->Test();
	GetLoggerFactory()->SaveAndCreateLogDirectory(L"C:\\LoggerTest\\");
	GetLoggerFactory()->Test();

	GetLoggerFactory()->GetLoggerProduct(L"Test1");
	GetLoggerFactory()->Test();
	system("pause");
	GetLoggerFactory()->GetLoggerProduct(L"Test2");
	ShowLoggers();
	GetLoggerFactory()->GetLoggerProduct(L"Test1");
	GetLoggerFactory()->GetLoggerProduct(L"Test3");

	
	TWLog_WARN(L"Test2", L"hello word!");
	GetLoggerFactory()->EnableConsole(true);
	ShowLoggers();
	GetLoggerFactory()->GetLoggerProduct(L"Test4");
	GetLoggerFactory()->GetLoggerProduct(L"Test5");
	GetLoggerFactory()->GetLoggerProduct(L"Test5");
	GetLoggerFactory()->GetLoggerProduct(L"Test5")->TraceDebug_f(L"enable test test");
	GetLoggerFactory()->GetLoggerProduct(L"Test5")->TraceInfor_f(L"enable test test test");
	GetLoggerFactory()->GetLoggerProduct(L"Test5");

	system("pause");
	GetLoggerFactory()->EnableConsole(false);

	GetLoggerFactory()->GetLoggerProduct(L"Test5")->TraceDebug_f(L"disable test test");
	GetLoggerFactory()->GetLoggerProduct(L"Test5")->TraceDebug_f(L"disable test test test");

	GetLoggerFactory()->GetLoggerProduct(L"Test1");
	GetLoggerFactory()->GetLoggerProduct(L"Test3");

	ShowLoggers();
#else
	GET_LOGGER()->EnableConsole(true);
	TwDbg(L"TEST: %s", L"dddd");
	TwError(L"Error: %s", L"dddd");
	TwTrace(L"Trace: %s", L"HERE");
	system("pause");
	GET_LOGGER()->EnableConsole(false);
#endif
	system("pause");

	Funct1();
	//Sleep(2000);
	Funct2();
	//Sleep(2000);
	Funct3();
	Funct4();
	ShowLoggers();
	return 0;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {

	return _tmain(__argc, __wargv);
}
