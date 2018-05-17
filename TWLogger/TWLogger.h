
/* ---------------------------------------------------------------------------------- *
*
* Copyright (c) 2017 Josephus <guifaliao@gmail.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files(the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify,
* merge, publish, distribute, sublicense, and / or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to the following
* conditions :
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* --------------------------------------------------------------------------------- */

#ifndef __TWINKLE_LOGGER_H__
#define __TWINKLE_LOGGER_H__

#pragma once

//#define TWINKLE_LOGGER_VERSION 2

#include "stdafx.h"
#include <Windows.h>
#include <fstream>
#include <string>
#include <sstream>
#include <tchar.h>
#include <io.h>
#include <direct.h>
#include <time.h>
#include <process.h>
#include <vector>
#include <map>



// When using Unicode Windows functions, use Unicode C-Runtime functions too.
#ifdef UNICODE
	#ifndef _UNICODE
		#define _UNICODE
	#endif
#endif

#define LAZY_MODEL

#define MAX_INFOR_LEN 4096
#define MAX_TIME_LEN 128
#define DEFALUT_INFOR_LEN 2048
#define MIN_INFOR_LEN 256
//#define	TIME_TO_WRITE_LOG (5 * 1000)	
#define	TIME_TO_WRITE_LOG (2 * 60 * 1000)



#define LEVEL_OVERALL_WRITEABLE 0xFFFF
#define LEVEL_LIMITED_WRITEABLE 0x00FF
#define LEVEL_TRACE_WRITEABLE 0x001F
#define LEVEL_INFOR_WRITEABLE 0x000F
#define LEVEL_DEBUG_WRITEABLE 0x0007
#define LEVEL_WARNING_WRITEABLE 0x0003
#define LEVEL_ERROR_WRITEABLE 0x0001
#define LEVEL_NONE_WRITEABLE 0x0000

typedef int LogLevel;



#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
#	define GET_LOGGER() (CTWLogger::GetInstance())
#else
	#include "TWSmartPointer.h"
#	define GET_LOGGER() (GetLoggerFactory()->GetLoggerProduct(tstring()))
#	define GET_LOG_INSTANCE(ndc) (GetLoggerFactory()->GetLoggerProduct(tstring(ndc)))
#   define TWLog_INFO(ndc, fmt, ...) \
		(GetLoggerFactory()->GetLoggerProduct(tstring(ndc)))->TraceInfor_f(fmt, ##__VA_ARGS__)
#   define TWLog_DEBUG(ndc, fmt, ...) \
		(GetLoggerFactory()->GetLoggerProduct(tstring(ndc)))->TraceDebug_f(fmt, ##__VA_ARGS__)
#   define TWLog_WARN(ndc, fmt, ...) \
		(GetLoggerFactory()->GetLoggerProduct(tstring(ndc)))->TraceWarning_f(fmt, ##__VA_ARGS__)
#   define TWLog_ERROR(ndc, fmt, ...) \
		(GetLoggerFactory()->GetLoggerProduct(tstring(ndc)))->TraceError_f(fmt, ##__VA_ARGS__)
#   define TWLog_TRACE(ndc, fmt, ...) \
		(GetLoggerFactory()->GetLoggerProduct(tstring(ndc)))->TraceTrace_f(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, fmt, ##__VA_ARGS__)
#endif //!(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)

#ifdef UNICODE
#	define WRITE_INFO_LOG (GET_LOGGER()->TraceInfor)
#	define WRITE_DEBUG_LOG (GET_LOGGER()->TraceDebug)
#	define WRITE_WARN_LOG (GET_LOGGER()->TraceWarning)
#	define WRITE_ERROR_LOG (GET_LOGGER()->TraceError)
#	define WRITE_ERRCODE_LOG (GET_LOGGER()->TraceErrWithLastCode)
#	define WRITE_TRACE_LOG(str) \
	GET_LOGGER()->TraceTrace(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, str)

#	define WRITE_INFO_PARAM (GET_LOGGER()->TraceInfor_f)
#	define WRITE_DEBUG_PARAM (GET_LOGGER()->TraceDebug_f)
#	define WRITE_WARN_PARAM (GET_LOGGER()->TraceWarning_f)
#	define WRITE_ERROR_PARAM (GET_LOGGER()->TraceError_f)

#	define WRITE_TRACE_PARAM_INNER_USE (GET_LOGGER()->TraceDefault_f)
#	define WRITE_TRACE_PARAM(fmt, ...) \
	GET_LOGGER()->TraceTrace_f(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, fmt, __VA_ARGS__)

#ifdef  NDEBUG
#define _TLOG_ASSERT(expr) \
	do { if (!expr){ \
	GET_LOGGER()->TraceAssert(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, \
	L"(\""#expr"\") Under NDEBUG !!!"); \
	}while(0)
#else
#	define _TLOG_ASSERT(expr) \
	do { if (!expr){ \
	GET_LOGGER()->TraceAssert(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, L"(\""#expr"\") !!!"); \
	if(1 == _CrtDbgReport(_CRT_ASSERT, (__FILE__), __LINE__, \
	NULL, #expr) ) \
	{_CrtDbgBreak(); \
	}}}while(0)
#endif

	#	define TLOG_TEXT2(STRING) L##STRING
	#	define TSTRING2 std::wstring
	#	define tfstream std::wfstream

#else
#	define WRITE_INFO_LOG (GET_LOGGER()->TraceInfor)
#	define WRITE_DEBUG_LOG (GET_LOGGER()->TraceDebug)
#	define WRITE_WARN_LOG (GET_LOGGER()->TraceWarning)
#	define WRITE_ERROR_LOG (GET_LOGGER()->TraceError)
#	define WRITE_ERRCODE_LOG (GET_LOGGER()->TraceErrWithLastCode)
#	define WRITE_TRACE_LOG(str) \
	GET_LOGGER()->TraceTrace(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, str)

#	define WRITE_INFO_PARAM (GET_LOGGER()->TraceInfor_f)
#	define WRITE_DEBUG_PARAM (GET_LOGGER()->TraceDebug_f)
#	define WRITE_WARN_PARAM (GET_LOGGER()->TraceWarning_f)
#	define WRITE_ERROR_PARAM (GET_LOGGER()->TraceError_f)

#	define WRITE_TRACE_PARAM_INNER_USE (GET_LOGGER()->TraceDefault_f)
#	define WRITE_TRACE_PARAM(fmt, ...) \
	GET_LOGGER()->TraceTrace_f(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, fmt, ##__VA_ARGS__)

#ifdef  NDEBUG
#define _TLOG_ASSERT(expr) \
	do { if (!expr){ \
	GET_LOGGER()->TraceAssert(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, \
	"(\""#expr"\") Under NDEBUG !!!"); \
	}while(0)
#else
#	define _TLOG_ASSERT(expr) \
	do { if (!expr){ \
	GET_LOGGER()->TraceAssert(_GetFileNameForLog(__FILE__), __LINE__, "(\""#expr"\") !!!"); \
	if(1 == _CrtDbgReport(_CRT_ASSERT, (__FILE__), __LINE__, \
	NULL, #expr) ) \
	{_CrtDbgBreak(); \
	}}}while(0)

#endif

#	define TLOG_TEXT2(STRING) STRING
#	define TSTRING2 std::string
#	define tfstream std::fstream

#endif //UNICODE

#define TLOG_TEXT(STRING) TLOG_TEXT2(STRING)
#define LOG_ASSERT(expr)  _TLOG_ASSERT(expr)

#if defined(UNICODE)
typedef std::wstring tstring;
typedef std::wstringstream tstringstream;
typedef wchar_t tchar;
#else
typedef std::string tstring;
typedef std::stringstream tstringstream;
typedef char tchar;
#endif

#	define TwInfo		(GET_LOGGER()->TraceInfor_f)
#	define TwDbg		(GET_LOGGER()->TraceDebug_f)
#	define TwWarn		(GET_LOGGER()->TraceWarning_f)
#	define TwError		(GET_LOGGER()->TraceError_f)
#	define TwLastErr	(GET_LOGGER()->TraceErrWithLastCode)
#	define TwTrace(fmt, ...) \
		GET_LOGGER()->TraceTrace_f(_GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, fmt, ##__VA_ARGS__)

//class CTWLogger;



const tstring DEFAULT_LOG_DIR = TLOG_TEXT("C:\\");
const tstring DEFAULT_LOG_NAME = TLOG_TEXT("Log.txt");

const tstring TRACE_INFOR = TLOG_TEXT("Infor: ");
const tstring TRACE_DEBUG = TLOG_TEXT("Debug: ");
const tstring TRACE_WARNNING = TLOG_TEXT("Warning: ");
const tstring TRACE_ERROR = TLOG_TEXT("Error: ");
const tstring TRACE_DEFAULT = TLOG_TEXT("Trace: ");
const tstring TRACE_TRACE = TLOG_TEXT("Trace: ");
const tstring TRACE_ASSERT = TLOG_TEXT("Assert: ");

enum FileModeEnum
{
	Default_Mode,
	Single_File_Mode,
	Day_Seperated_Mode,
	Module_Seperated_Mode,
	MAX_Mode
};

enum DateTypeEnum
{
	For_Record_Type,
	For_File_Type
};

enum 
{
	/*path//directory//program.exe*/
	MODE_DEFAULT,
	/*path//directory//*/
	MODE_DIRECTORY,
	/*program.exe*/
	MODE_FILENAME,
	/*program*/
	MODE_FILENAME_ONLY,
	MODE_FILENAME_WITH_PATH,
	MODE_MAX
};

#if (defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)

class CTWLoggerShell;

typedef std::vector<CTWLoggerShell> LoggerList;
typedef std::map<tstring, CTWLoggerShell> LoggerMap;

#endif //(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)

class CTWLogger
#if (defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
	 : public TWSharedObject
#endif 
	
{
public:
#if (defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
	CTWLogger(const tstring& name, const tchar* lpszLogSummaryDir = TLOG_TEXT(""));
#endif //(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)

	~CTWLogger(void);
#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
	/** lpszDir is reserved. */
	static void InitLog(FileModeEnum eMode = Day_Seperated_Mode, const tchar* lpszDir = TLOG_TEXT(""));
	static void ReleaseLog();
	static CTWLogger* GetInstance();
	static CRITICAL_SECTION s_cs;
	static int bInitialized;
	static bool IsEnableLogg() { return (bInitialized != 0); }
	static void EndLog();
	static void SetFileWriteMode(FileModeEnum eMode = Day_Seperated_Mode,
		const tchar* lpszDir = TLOG_TEXT(""));
#else
	bool IsEnableLogg() const
	{ return !(m_usWriteStatus == LEVEL_NONE_WRITEABLE); }
	//void SetLogParentDirectory(const tchar* lpszdir);
	void SetFileWriteMode(FileModeEnum eMode = Day_Seperated_Mode,
		const tchar* lpszDir = TLOG_TEXT(""));
#endif

	static void DelayLoop(unsigned long usec);
	static DWORD GetCurExeNameOrPath(tchar* outFilePath, int sizeLen, int fetchKind);
	
	void TraceInfor(const tchar* strInfo);
	void TraceDebug(const tchar* strInfo);
	void TraceWarning(const tchar* strInfo);
	void TraceError(const tchar* strInfo);
	void TraceDefault(const tchar* strInfo);
	void TraceTrace(const tchar* szFile, int line, const tchar* strInfo);
	void TraceAssert(const tchar* szFile, int line, const tchar* strInfo);

	void TraceInfor_f(const tchar* fmt, ...);
	void TraceDebug_f(const tchar* fmt, ...);
	void TraceWarning_f(const tchar* fmt, ...);
	void TraceError_f(const tchar* fmt, ...);
	void TraceDefault_f(const tchar* fmt, ...);
	void TraceTrace_f(const tchar* szFile, int line, const tchar* fmt, ...);

	/*
	*Account for GetLastError() with plain text.
	*@param erroCode: the error code which would be explained. if 0 function would invoke 
	*GetLastError() Api inner.
	*/
	int FormatLastError(tchar* szOutMsg, unsigned int sizeLen, DWORD erroCode = (DWORD)-1);
	void TraceErrWithLastCode(const tchar* strInfo);

	//Set log file name with inner static value, this function always returned true.
	bool RefleshFileName() {
		if(m_eFileMode != Day_Seperated_Mode) {
			m_fileName = m_strLogDirOrFilePath;
		}
		return true;
	}
	tstring GetFileName();

	//0x1111, 0x0111, 0x0011, 0x0001, 0x0000
	void SetWriteAbleLevel(USHORT value = LEVEL_INFOR_WRITEABLE) { m_usWriteStatus = value; }
	void DisableRecord() { m_usWriteStatus = LEVEL_NONE_WRITEABLE; }
	void SetRecordNoBuffer() {
		if(!m_bWriteRealTime && GetTimerEvent() != NULL) {
			SetEvent(hEventWrite);
		}
		m_bWriteRealTime = true;
	}
	bool EnableConsole(bool bEnable = true) {
#ifdef _DEBUG
#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
		if (!m_bCreateConsole && bEnable) {
			if (!AllocConsole())
				return false;
			FILE* here = NULL;
			freopen_s(&here, "CONOUT$", "w", stdout);
			freopen_s(&here, "CONOUT$", "w", stderr);
			m_bCreateConsole = true;
			return true;
		}
		if (m_bCreateConsole && !bEnable) {
			if (!FreeConsole())
				return false;
			m_bCreateConsole = false;
			return true;
		}
#else
		m_bCreateConsole = bEnable;
#endif
#endif //DEBUG
		return true;
	}
	HANDLE GetTimerEvent() {
		if(hEventWrite && hEventWrite != INVALID_HANDLE_VALUE)
			return hEventWrite;
		return NULL;
	}

	void Start();
	__int64 Now() const;
	__int64 NowInMicro() const;

private:
#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
	class CAutoWriteHelper;
	friend CAutoWriteHelper;
	static CTWLogger* m_pLogger;
	static tstring m_strLogDirOrFilePath;
#else
	tstring m_strLogDirOrFilePath;
#endif //!(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)

	CTWLogger();
	tstring m_fileName;
	tfstream m_logStream;
	tstring GetCustomTime(DateTypeEnum type = For_Record_Type);
	tstring GetTimeOfDay();
	void Trace(tstring strInfo);
	void Console(tstring strInfo);
	void Trace_format(const tchar* fmt, va_list list_arg);
	bool WriteToFile(tstring str);
	bool ClearToFile();
	
	USHORT m_usWriteStatus;
	tstring m_strBuffer;
	unsigned int m_bufCurLen;
	static FileModeEnum m_eFileMode;
	
	
	HANDLE hMutexForBuffer;
	HANDLE hEventWrite;

	bool m_bWriteRealTime;
	bool m_bCreateConsole;

	LARGE_INTEGER m_liPerfFreq;
	LARGE_INTEGER m_liPerfStart;

#if (defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
public:
	tstring GetName() const { return m_instanceName; }
	CRITICAL_SECTION s_cs;
private:
	// Disallow copying of instances of this class
	CTWLogger(const CTWLogger&);
	CTWLogger& operator = (const CTWLogger& );

	friend class CTWLoggerShell;
private:
	tstring m_instanceName;

#else

private:
	/*
	http://bbs.csdn.net/topics/390648143
	*/
	class CAutoWriteHelper
	{
	public:
		CAutoWriteHelper():m_pLog(NULL),m_threadHandle(NULL)
		{
			if(CTWLogger::InitLog(), m_pLog = CTWLogger::GetInstance())
			{
				unsigned int threadID = 0;
				m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, 
					CTWLogger::CAutoWriteHelper::TimerWriteProc,
					this, 0, &threadID);
			}
		}
		~CAutoWriteHelper()
		{
			if(CTWLogger::m_pLogger != NULL)
			{
				if(m_pLogger->GetTimerEvent() != NULL)
				{
					SetEvent(m_pLog->hEventWrite);
					WaitForSingleObject(m_threadHandle, INFINITE);
					CloseHandle(m_threadHandle);
					m_threadHandle = NULL;
				}
				m_pLog = NULL;
				CTWLogger::EndLog();
			}
		}
		static unsigned int _stdcall TimerWriteProc(void* param);

	private:
		CTWLogger* m_pLog;
		HANDLE m_threadHandle;
		void WriteToLog()
		{
			if(m_pLog)
			{
				m_pLog->ClearToFile();
			}
		}
		HANDLE GetLogTimerEvent()
		{
			if(m_pLog)
			{
				return m_pLogger->GetTimerEvent();
			}
			return NULL;
		}
	};
	static CAutoWriteHelper helper;

#endif //(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
};

#if (defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)

class LoggerFactory;

class CTWLoggerShell
{
public:

	static CTWLoggerShell GetInstance(const tstring& name);
	static LoggerList GetCurrentLoggers();
	static bool Exists(const tstring& name);
	static CTWLoggerShell GetDefaultLogger();

	CTWLoggerShell()
		:swaddle(nullptr)
	{}
	CTWLoggerShell(CTWLogger* ptr)
		:swaddle(ptr)
	{
		if (swaddle)
			_tprintf(TLOG_TEXT("CTWLoggerShell::Constructor: %s\n"), swaddle->GetName().c_str());
		if (swaddle)
			swaddle->AddRef();
	}
	CTWLoggerShell(const CTWLoggerShell& rhs)
		: swaddle(rhs.swaddle)
	{
		if (swaddle)
			_tprintf(TLOG_TEXT("CTWLoggerShell::Constructor1: %s\n"), swaddle->GetName().c_str());
		if (swaddle)
			swaddle->AddRef();
	}
	CTWLoggerShell & operator = (const CTWLoggerShell& rhs)
	{
		if (swaddle)
			_tprintf(TLOG_TEXT("CTWLoggerShell::Constructor2: %s\n"), swaddle->GetName().c_str());
		CTWLoggerShell(rhs).Swap(*this);
		return *this;
	}
	~CTWLoggerShell()
	{
		if (swaddle)
			_tprintf(TLOG_TEXT("CTWLoggerShell::Destructor: %s\n"), swaddle->GetName().c_str());
		if (swaddle)
			swaddle->RemoveRef();
	}
	void Swap(CTWLoggerShell & other)
	{
		using std::swap;
		swap(swaddle, other.swaddle);
	}
	//LogLevel GetLogLevel() const
	//{
	//	return swaddle->GetLogLevel();
	//}
	//void SetLogLevel(LogLevel ll)
	//{
	//	swaddle->SetLogLevel(ll);
	//}
	tstring GetName() const
	{
		return swaddle->GetName();
	}

	const CTWLogger* operator->() const 
	{
		if (swaddle) return swaddle;
		else return nullptr;
	}
	CTWLogger* operator->()
	{
		if (swaddle) return swaddle;
		else return nullptr;
	}
	bool IsFillFlesh() const
	{
		if (swaddle) return true;
		return false;
	}
private:
	CTWLogger * swaddle;
};

class LoggerFactory
{
public:
	LoggerFactory() 
		:m_strLogDirectory(TLOG_TEXT(""))
		, m_bConsoleAppend(false)
	{
		/*if (!m_defaultLogger.IsFillFlesh())*/ {
			tchar szExeName[MAX_PATH] = { 0 };
			if (CTWLogger::GetCurExeNameOrPath(szExeName, MAX_PATH, MODE_FILENAME_ONLY) >= 0) {
				m_defaultLogger = CreateNewLoggerInstance(szExeName);
			}
			if (CTWLogger::GetCurExeNameOrPath(szExeName, MAX_PATH, MODE_DIRECTORY) >= 0) {
				m_strLogDirectory = szExeName;
			}
		}
	}
	~LoggerFactory() {
		if (m_bConsoleAppend) {
			FreeConsole();
			m_bConsoleAppend = false;
		}
	}

	CTWLoggerShell GetLoggerProduct(const tstring& name)
	{
		CTWLoggerShell logger;
		LoggerMap::iterator iter;
		if (name.empty())
		{
			//TODO:
			logger = m_defaultLogger;
		}
		else if ((iter = m_ActivedLoggers.find(name)) != m_ActivedLoggers.end())
		{
			logger = iter->second;
		}
		else
		{
			logger = CreateNewLoggerInstance(name);
			logger->EnableConsole(m_bConsoleAppend);
			//m_ActivedLoggers.insert(std::pair<tstring, CTWLoggerShell>);
			std::pair<LoggerMap::iterator, bool> ret = m_ActivedLoggers.insert(make_pair(name, logger));
		}
		return logger;
	}
	CTWLoggerShell CreateNewLoggerInstance(const tstring& name)
	{
		return CTWLoggerShell(new CTWLogger(name, m_strLogDirectory.c_str()));
	}

	void InitializeLoggerList(LoggerList& list)
	{
		list.reserve(list.size() + m_ActivedLoggers.size());
		LoggerMap::const_iterator citer = m_ActivedLoggers.cbegin();
		while (citer != m_ActivedLoggers.cend())
		{
			list.push_back(citer->second);
			citer++;
		}
	}

	void InitializeDefaultLogger(CTWLoggerShell& logger)
	{
		logger = m_defaultLogger;
	}
	void Test() {
#ifdef UNICODE
		_tprintf(TLOG_TEXT("directory: %ws\n"), m_strLogDirectory.c_str());
#else
		_tprintf(TLOG_TEXT("directory: %s\n"), m_strLogDirectory.c_str());
#endif
	}
	bool SaveAndCreateLogDirectory(const tchar* lpszPath)
	{
		size_t len = 0;
		if (lpszPath == NULL || (len = _tcslen(lpszPath)) == 0)
			return false;
		if (len == 1)
			if (!((lpszPath[0] <= TLOG_TEXT('z') && lpszPath[0] >= TLOG_TEXT('a'))
				|| (lpszPath[0] <= TLOG_TEXT('Z') && lpszPath[0] >= TLOG_TEXT('A'))))
				return false;
		if (len >= 2 && lpszPath[1] != TLOG_TEXT(':'))
			return false;
		if (len >= 3 && (lpszPath[2] != TLOG_TEXT('\\') && lpszPath[2] != TLOG_TEXT('/')))
			return false;
		if (len <= 3)
		{
			tchar disk = lpszPath[0];
			if (disk >= TLOG_TEXT('a') && disk <= TLOG_TEXT('z')) disk -= 32;
			int no = disk - TLOG_TEXT('A');
			DWORD dwRes = GetLogicalDrives();
			if ((dwRes & (1 << no)) == 0)
				return false;
			m_strLogDirectory.assign(1, disk);
			m_strLogDirectory.append(TLOG_TEXT(":"));
			return true;
		}

		tchar* path = new tchar[len + 2];
		memset(path, 0, sizeof(tchar)*(len + 2));
		memcpy(path, lpszPath, len*sizeof(tchar));
		path[len] = TLOG_TEXT('\0');
		int pos = len - 1;
		for (; pos >= 0 && (path[pos] == TLOG_TEXT('\\') || path[pos] == TLOG_TEXT('/')); --pos){;}
		path[pos + 1] = TLOG_TEXT('\0');
		path[pos + 2] = TLOG_TEXT('\0');
		len = _tcslen(path);

		WIN32_FIND_DATA wfd = { 0 };
		HANDLE hFile = FindFirstFile(path, &wfd);
		path[pos + 1] = TLOG_TEXT('\\');
		if (hFile != INVALID_HANDLE_VALUE) {
			FindClose(hFile);
		}
		if (hFile == INVALID_HANDLE_VALUE || !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
			_tprintf(TLOG_TEXT("Create log directory.\n"));
			tchar *p = &path[0];
			while ((p = _tcsrchr(p, TLOG_TEXT('\\'))) != NULL) {
				*p = 0;
				DWORD dwRes = GetFileAttributes(path);
				if (!(dwRes != INVALID_FILE_ATTRIBUTES && (dwRes & FILE_ATTRIBUTE_DIRECTORY))) {
					if (!CreateDirectory(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
						delete[] path;
						return false;
					}
				}
				*p = TLOG_TEXT('\\');
				p++;
			}
		}
		path[pos + 1] = TLOG_TEXT('\0');
		m_strLogDirectory.assign(path);
		delete[] path;
		if(m_defaultLogger.IsFillFlesh()) {
			m_defaultLogger->SetFileWriteMode(Module_Seperated_Mode, m_strLogDirectory.c_str());
		}
		return true;
	}

	bool EnableConsole(bool bEnable = true) {
#ifdef _DEBUG
		bool bUpdate = false;
		if (!m_bConsoleAppend && bEnable) {
			if (!AllocConsole())
				return false;
			FILE* here = NULL;
			freopen_s(&here, "CONOUT$", "w", stdout);
			freopen_s(&here, "CONOUT$", "w", stderr);
			m_bConsoleAppend = true;
			bUpdate = true;
		}
		else if (m_bConsoleAppend && !bEnable) {
			if (!FreeConsole())
				return false;
			m_bConsoleAppend = false;
			bUpdate = true;
		}
		if (bUpdate) {
			m_defaultLogger->EnableConsole(bEnable);
			LoggerMap::iterator iter;
			for (iter = m_ActivedLoggers.begin(); iter != m_ActivedLoggers.end(); ++iter) {
				iter->second->EnableConsole(bEnable);
			}
		}
#endif //_DEBUG
		return true;
	}

private:
	LoggerFactory(const LoggerFactory&);
	LoggerFactory& operator=(const LoggerFactory&);

	LoggerMap m_ActivedLoggers;
	CTWLoggerShell m_defaultLogger;

	bool m_bConsoleAppend;
	tstring m_strLogDirectory;
};

//Declare global variable.
extern LoggerFactory logger_factory;
static LoggerFactory* GetLoggerFactory() { return &logger_factory; }

#endif //(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)

namespace TwinkleLib {
	
	class TraceLogger
	{
	public:
		TraceLogger(const tchar* message, const tchar* fileName, int nLine)
			:m_pszMes(message), m_pszFileN(fileName), m_nLine(nLine), m_pnRet(NULL), m_pDwRet(NULL), m_retType(-1)
		{
			if(GET_LOGGER()->IsEnableLogg())
			{
				WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("Enter {%s}, file: {%s}, line: {%d}."), m_pszMes, m_pszFileN, m_nLine);
			}
		}
		TraceLogger(const tchar* message, const tchar* fileName, int nLine, int* pRet)
			:m_pszMes(message), m_pszFileN(fileName), m_nLine(nLine)
			, m_pnRet(pRet), m_pDwRet(NULL), m_pfRet(NULL), m_retType(0)
		{
			if(GET_LOGGER()->IsEnableLogg())
			{
				WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("Enter {%s}, file: {%s}, line: {%d}."), m_pszMes, m_pszFileN, m_nLine);
			}
		}
		TraceLogger(const tchar* message, const tchar* fileName, int nLine, PDWORD pRet)
			:m_pszMes(message), m_pszFileN(fileName), m_nLine(nLine)
			, m_pnRet(NULL), m_pDwRet(pRet), m_pfRet(NULL), m_retType(1)
		{
			if(GET_LOGGER()->IsEnableLogg())
			{
				WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("Enter {%s}, file: {%s}, line: {%d}."), m_pszMes, m_pszFileN, m_nLine);
			}
		}
		//TraceLogger(const tchar* message, const tchar* fileName, int nLine, PBOOL pRet)
		//	:m_pszMes(message), m_pszFileN(fileName), m_nLine(nLine)
		//	, m_pnRet(NULL), m_pDwRet(NULL), m_pfRet(pRet), m_retType(2)
		//{
		//	if (GET_LOGGER()->IsEnableLogg()) {
		//		WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("Enter {%s}, file: {%s}, line: {%d}."), m_pszMes, m_pszFileN, m_nLine);
		//	}
		//}
		~TraceLogger()
		{
			if(GET_LOGGER()->IsEnableLogg())
			{
				if(m_retType == 0)
				{
					WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("Leave {%s}, file: {%s}, line: {%d}, return: {%d}."), 
						m_pszMes, m_pszFileN, m_nLine, *m_pnRet);
				}
				else if(m_retType == 1)
				{
					WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("Leave {%s}, file: {%s}, line: {%d}, return: {%u}."), 
						m_pszMes, m_pszFileN, m_nLine, *m_pDwRet);
				}
				else if (m_retType == 2)
				{
					WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("Leave {%s}, file: {%s}, line: {%d}, return: {%d}."), 
						m_pszMes, m_pszFileN, m_nLine, *m_pfRet);
				}
				else {
					WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("Leave {%s}, file: {%s}, line: {%d}."),
						m_pszMes, m_pszFileN, m_nLine);
				}
			}
		}
	private:

		TraceLogger (TraceLogger const &);
		TraceLogger & operator = (TraceLogger const &);

		const tchar* m_pszMes;
		const tchar* m_pszFileN;
		int m_nLine;

		int* m_pnRet;
		PDWORD m_pDwRet;
		BOOL* m_pfRet;
		int m_retType;
	};

}

const tchar *_GetFileNameForLog(const tchar *pszFilePath);
#define LOG_FUNCTION() TwinkleLib::TraceLogger _FunctionTraceLogger(\
	TLOG_TEXT(__FUNCTION__), _GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__)

#define TRACE_FUNCTION(pValue) TwinkleLib::TraceLogger _FunctionTraceLogger(\
	TLOG_TEXT(__FUNCTION__), _GetFileNameForLog(TLOG_TEXT(__FILE__)), __LINE__, (pValue))

#endif //__TWINKLE_LOGGER_H__