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



#include "stdafx.h"
#include "TWLogger.h"

FileModeEnum CTWLogger::m_eFileMode = Default_Mode;

#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
int CTWLogger::bInitialized = 0;
CTWLogger* CTWLogger::m_pLogger = NULL;
CRITICAL_SECTION CTWLogger::s_cs;
tstring CTWLogger::m_strLogDirOrFilePath = DEFAULT_LOG_DIR;
CTWLogger::CAutoWriteHelper CTWLogger::helper;
#endif

#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)

CTWLogger::CTWLogger(void)
	:m_bWriteRealTime(true),
	m_fileName(DEFAULT_LOG_NAME),
	m_usWriteStatus(LEVEL_OVERALL_WRITEABLE),
	m_bufCurLen(0),
	hMutexForBuffer(NULL),
	hEventWrite(NULL)
{
	QueryPerformanceFrequency(&m_liPerfFreq);
	m_strBuffer = TLOG_TEXT("");
	SetFileWriteMode(Day_Seperated_Mode);
	hMutexForBuffer = CreateMutex(NULL, FALSE, LPCTSTR("iGobalBuffer"));
	if(!m_bWriteRealTime)
	{
		hEventWrite = CreateEvent(NULL, TRUE, FALSE, TLOG_TEXT("iGobalTimerWriteEvent"));
	}
}

#else

CTWLogger::CTWLogger(void)
	:m_name(nullptr),
	m_bWriteRealTime(true),
	m_fileName(nullptr),
	m_usWriteStatus(LEVEL_NONE_WRITEABLE),
	m_bufCurLen(0),
	hMutexForBuffer(NULL),
	hEventWrite(NULL)
{
	QueryPerformanceFrequency(&m_liPerfFreq);
	m_strBuffer = TLOG_TEXT("");
	SetFileWriteMode(Day_Seperated_Mode);
	hMutexForBuffer = CreateMutex(NULL, FALSE, LPCTSTR("iGobalBuffer"));
	if(!m_bWriteRealTime)
	{
		hEventWrite = CreateEvent(NULL, TRUE, FALSE, TLOG_TEXT("iGobalTimerWriteEvent"));
	}
}

CTWLogger::CTWLogger(const tstring& name, const tchar* lpszLogSummaryDir)
	:m_name(name),
	m_bWriteRealTime(true),
	m_fileName(name),
	m_usWriteStatus(LEVEL_OVERALL_WRITEABLE),
	m_bufCurLen(0),
	hMutexForBuffer(NULL),
	hEventWrite(NULL)
{
	InitializeCriticalSection(&s_cs);
	QueryPerformanceFrequency(&m_liPerfFreq);
	m_strBuffer = TLOG_TEXT("");
	SetFileWriteMode(Module_Seperated_Mode, lpszLogSummaryDir);
	tstring mtxName(name);
	mtxName += TLOG_TEXT("iGobalBuffer");
	hMutexForBuffer = CreateMutex(NULL, FALSE, mtxName.c_str());
	if(!m_bWriteRealTime) {
		mtxName = name + TLOG_TEXT("TimerWriteEvent");
		hEventWrite = CreateEvent(NULL, TRUE, FALSE, mtxName.c_str());
	}
	_tprintf(TLOG_TEXT("-------------------------CTWLogger::Contrustor: %s: \n"), m_name.c_str());
}

#endif

CTWLogger::~CTWLogger(void)
{
	WaitForSingleObject(hMutexForBuffer, INFINITE);
	if(m_strBuffer.length() > 0)
	{
		bool retFlag = false;
		if(m_eFileMode == Day_Seperated_Mode)
		{
			m_fileName = GetCustomTime(For_File_Type);
		}
		retFlag = WriteToFile(m_strBuffer);
		if(retFlag)
		{
			m_strBuffer = TLOG_TEXT("");
			m_bufCurLen = 0;
		}
	}
	ReleaseMutex(hMutexForBuffer);
	if(hMutexForBuffer && hMutexForBuffer != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMutexForBuffer);
		hMutexForBuffer = NULL;
	}
	if(hEventWrite && hEventWrite != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hEventWrite);
		hEventWrite = NULL;
	}

	//TODO: Bug thirsty to fix !!! -Josephus@2017913 8:29:40
#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
	//bInitialized = 0;
#else
	DeleteCriticalSection(&s_cs);
#endif
}

tstring CTWLogger::GetCustomTime(DateTypeEnum type)
{
	tchar strTime[MAX_TIME_LEN] = {0};
	time_t now;
	time(&now);
	tm *pTm = new tm;
	localtime_s(pTm, &now);

	if(type == For_File_Type)
	{
		_tcsftime(strTime, MAX_TIME_LEN*sizeof(tchar), TLOG_TEXT("%Y%m%d.log"), pTm);

#ifdef LAZY_MODEL
#ifndef UNICODE
		if(_access(m_strLogDirOrFilePath.c_str(), 0) == -1)
#else
		char tlsDirectory[MAX_PATH] = {0};
		int nt = WideCharToMultiByte(CP_ACP, 0, m_strLogDirOrFilePath.c_str(),
			-1, tlsDirectory, MAX_PATH, NULL, NULL);
		if(_access(tlsDirectory, 0) == -1)
#endif
		{
			CreateDirectory(m_strLogDirOrFilePath.c_str(), NULL);
		}

#endif
		return (m_strLogDirOrFilePath + TLOG_TEXT("\\") + strTime);
	}
	else if(type == For_Record_Type)
	{
		_tcsftime(strTime, MAX_TIME_LEN*sizeof(tchar), TLOG_TEXT("[%Y/%m/%d %H:%M:%S] "), pTm);
		return tstring(strTime);
	}
	
	return TLOG_TEXT("");
}

tstring CTWLogger::GetTimeOfDay()
{
	FILETIME time;
	double timed;
	tchar strTime[MAX_TIME_LEN] = {0};

	GetSystemTimeAsFileTime( &time );

	// Apparently Win32 has units of 1e-7 sec (tenths of microsecs)
	// 4294967296 is 2^32, to shift high word over
	// 11644473600 is the number of seconds between
	// the Win32 epoch 1601-Jan-01 and the Unix epoch 1970-Jan-01
	// Tests found floating point to be 10x faster than 64bit int math.

	timed = ((time.dwHighDateTime * 4294967296e-7) - 11644473600.0) +
		(time.dwLowDateTime  * 1e-7);

	tm *pTm = new tm;
	long temp = (long) timed;
	time_t clock = (long) timed;
	long appendix = (long) ((timed - temp) * 1e6);
	localtime_s(pTm, &clock);

	if(m_eFileMode == Day_Seperated_Mode || m_eFileMode == Module_Seperated_Mode)
	{
		_tcsftime(strTime, MAX_TIME_LEN, TLOG_TEXT("[%H:%M:%S"), pTm);
	}
	else
	{
		_tcsftime(strTime, MAX_TIME_LEN, TLOG_TEXT("[%Y/%m/%d %H:%M:%S"), pTm);
	}
	int nowLen = _tcslen(strTime);
	_stprintf_s(strTime+nowLen, MAX_TIME_LEN-nowLen, TLOG_TEXT(",%03d] "), appendix / 1000);
	return tstring(strTime);
}

void CTWLogger::Trace(tstring strInfo)
{
	bool retFlag = false;
	WaitForSingleObject(hMutexForBuffer, INFINITE);
	m_strBuffer += strInfo;
	m_strBuffer += TLOG_TEXT("\r\n");
	if(m_bWriteRealTime || m_strBuffer.length() > MAX_INFOR_LEN)
	{
		if(m_eFileMode == Day_Seperated_Mode || m_eFileMode == Module_Seperated_Mode)
		{
			m_fileName = GetCustomTime(For_File_Type);
		}

		//MessageBox(NULL, TLOG_TEXT(m_fileName.c_str()), 
		//	TLOG_TEXT(""), MB_ICONINFORMATION | MB_OK);

		retFlag = WriteToFile(m_strBuffer);
		if(retFlag)
		{
			m_strBuffer = TLOG_TEXT("");
			m_bufCurLen = 0;
		}
	}
	ReleaseMutex(hMutexForBuffer);
}
#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
CTWLogger* CTWLogger::GetInstance()
{
	EnterCriticalSection(&s_cs);
	if(m_pLogger == NULL)
	{
		m_pLogger = new CTWLogger();
		//if(m_pLogger)
		//{
		//	unsigned int threadID = 0;
		//	HANDLE threadHD = (HANDLE)_beginthreadex(NULL, 0, CTWLogger::TimerWriteProc, m_pLogger, 0, &threadID);
		//	if(threadHD && threadHD != INVALID_HANDLE_VALUE)
		//	{
		//		CloseHandle(threadHD);
		//	}
		//}
	}
	LeaveCriticalSection(&s_cs);
	return m_pLogger;
}

void CTWLogger::InitLog(FileModeEnum eMode, const tchar* lpszDir)
{
	if(bInitialized == 0)
	{
		InitializeCriticalSection(&s_cs);
		//SetFileWriteMode(eMode, lpszDir);
		bInitialized = 1;
	}
}

void CTWLogger::ReleaseLog()
{
	if(bInitialized) 
	{
		DeleteCriticalSection(&s_cs);
		bInitialized = 0;
	}
}

void CTWLogger::EndLog()
{
	if(m_pLogger != NULL)
	{
		delete m_pLogger;
		m_pLogger = NULL;
	}
}

#endif

void CTWLogger::SetFileWriteMode(FileModeEnum eMode, const tchar* lpszDir)
{
	tchar fileName[MAX_PATH] = {0};
	if(eMode == Single_File_Mode) {
		if(lpszDir == NULL || _tcslen(lpszDir) <= 0) {
			if(GetCurExeNameOrPath(fileName, MAX_PATH, MODE_FILENAME_WITH_PATH) != -1) {
				_tcscat_s(fileName, MAX_PATH*sizeof(tchar), TLOG_TEXT("_log.txt"));
				m_strLogDirOrFilePath = fileName;
				m_eFileMode = Single_File_Mode;
#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
				if(m_pLogger != NULL && m_pLogger->RefleshFileName()){;}
#endif
				return;
			}
		}
		else {
			if(GetCurExeNameOrPath(fileName, MAX_PATH, MODE_FILENAME_ONLY) != -1) {
				_tcscat_s(fileName, MAX_PATH*sizeof(tchar), TLOG_TEXT("_log.txt"));
				m_strLogDirOrFilePath = lpszDir;
				const tchar* pchSuffix = _tcsrchr(lpszDir, TLOG_TEXT('\\'));
				if(pchSuffix == NULL || pchSuffix[1] != '\0') {
					m_strLogDirOrFilePath += tstring(TLOG_TEXT("\\"));
				}
				m_strLogDirOrFilePath += fileName;
				m_eFileMode = Single_File_Mode;
#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
				if(m_pLogger != NULL && m_pLogger->RefleshFileName()){;}
#endif
				return;
			}
		}
	}
	else if(eMode == Day_Seperated_Mode)
	{
		if(lpszDir == NULL || _tcslen(lpszDir) <= 0) {
			if(GetCurExeNameOrPath(fileName, MAX_PATH, MODE_DIRECTORY) != -1) {
				m_strLogDirOrFilePath = tstring(fileName) + TLOG_TEXT("LogFiles");
			}
		} else {
			const tchar* pchSuffix = _tcsrchr(lpszDir, TLOG_TEXT('\\'));
			if (pchSuffix != NULL && pchSuffix[1] == TLOG_TEXT('\0')) {
				_tcscpy_s(fileName, MAX_PATH*sizeof(tchar), lpszDir);
				fileName[_tcslen(fileName)-1] = TLOG_TEXT('\0');
				m_strLogDirOrFilePath = tstring(fileName);
			} else {
				m_strLogDirOrFilePath = lpszDir;
			}
			//MessageBox(NULL, TLOG_TEXT(m_strLogDirOrFilePath.c_str()), 
			//	TLOG_TEXT(""), MB_ICONINFORMATION | MB_OK);
		}

#ifndef LAZY_MODEL
#ifndef UNICODE
		if(_access(m_strLogDirOrFilePath.c_str(), 0) == -1)
#else
		char tlsDirectory[MAX_PATH] = {0};
		int nt = WideCharToMultiByte(CP_ACP, 0, m_strLogDirOrFilePath.c_str(),
			-1, tlsDirectory, MAX_PATH, NULL, NULL);
		if(_access(tlsDirectory, 0) == -1)
#endif
		{
			CreateDirectory(m_strLogDirOrFilePath.c_str(), NULL);
		}
#endif
		m_eFileMode = Day_Seperated_Mode;
	}
	else if(eMode == Default_Mode)
	{
		m_eFileMode = Default_Mode;
		m_strLogDirOrFilePath = DEFAULT_LOG_NAME;
#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
		if(m_pLogger != NULL && m_pLogger->RefleshFileName()){;}
#endif
	}
#if (defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)
	else if(eMode == Module_Seperated_Mode) {
		if(m_name.length() <= 0) {
			_tprintf(TLOG_TEXT("module name is invalid.\n"));
			return;
		}
		if(lpszDir == NULL || _tcslen(lpszDir) <= 0) {
			if(GetCurExeNameOrPath(fileName, MAX_PATH, MODE_DIRECTORY) != -1) {
				m_strLogDirOrFilePath = tstring(fileName) + TLOG_TEXT("LogFiles");
			}else { 
				_tprintf(TLOG_TEXT("GetCurExeNameOrPath failed: %s"), fileName);
				return; 
			}
		}else {
			const tchar* pchSuffix = _tcsrchr(lpszDir, TLOG_TEXT('\\'));
			if (pchSuffix != NULL && pchSuffix[1] == TLOG_TEXT('\0')) {
				_tcscpy_s(fileName, MAX_PATH*sizeof(tchar), lpszDir);
				fileName[_tcslen(fileName)-1] = TLOG_TEXT('\0');
				m_strLogDirOrFilePath = tstring(fileName);
			}else {
				m_strLogDirOrFilePath = lpszDir;
			}
		}
		m_strLogDirOrFilePath += TLOG_TEXT("\\");
		m_strLogDirOrFilePath += m_name;
#ifndef LAZY_MODEL
#ifndef UNICODE
		if(_access(m_strLogDirOrFilePath.c_str(), 0) == -1) {
#else
		char tlsDirectory[MAX_PATH] = {0};
		int nt = WideCharToMultiByte(CP_ACP, 0, m_strLogDirOrFilePath.c_str(),
			-1, tlsDirectory, MAX_PATH, NULL, NULL);
		if(_access(tlsDirectory, 0) == -1) {
#endif
			CreateDirectory(m_strLogDirOrFilePath.c_str(), NULL);
		}
#endif
		m_eFileMode = Module_Seperated_Mode;
	}
#endif

}

void CTWLogger::TraceInfor(const tchar* strInfo)
{
	if((m_usWriteStatus & LEVEL_INFOR_WRITEABLE) != LEVEL_INFOR_WRITEABLE) return;
	tstring str(GetTimeOfDay()+TRACE_INFOR);
	str += strInfo;
	Trace(str);
}

void CTWLogger::TraceDebug(const tchar* strInfo)
{
	if((m_usWriteStatus & LEVEL_DEBUG_WRITEABLE) != LEVEL_DEBUG_WRITEABLE) return;
	tstring str(GetTimeOfDay()+TRACE_DEBUG);
	str += strInfo;
	Trace(str);
}

void CTWLogger::TraceWarning(const tchar* strInfo)
{
	if((m_usWriteStatus & LEVEL_WARNING_WRITEABLE) != LEVEL_WARNING_WRITEABLE) return;
	tstring str(GetTimeOfDay() + TRACE_WARNNING);
	str += strInfo;
	Trace(str);
}

void CTWLogger::TraceError(const tchar* strInfo)
{
	if((m_usWriteStatus & LEVEL_ERROR_WRITEABLE) != LEVEL_ERROR_WRITEABLE) return;
	tstring str(GetTimeOfDay() + TRACE_ERROR);
	str += strInfo;
	Trace(str);
}

void CTWLogger::TraceDefault(const tchar* strInfo)
{
	if((m_usWriteStatus & LEVEL_LIMITED_WRITEABLE) != LEVEL_LIMITED_WRITEABLE) return;
	tstring str(GetTimeOfDay() + TRACE_DEFAULT);
	str += strInfo;
	Trace(str);
}

void CTWLogger::TraceTrace(const tchar* szFile, int line, const tchar* strInfo)
{
	if((m_usWriteStatus & LEVEL_TRACE_WRITEABLE) != LEVEL_TRACE_WRITEABLE) return;
	tstring str(GetTimeOfDay() + TRACE_TRACE);
	tstringstream tss;
	tss << line;
	str += strInfo;
	str = str + TLOG_TEXT(" file: {") + szFile + TLOG_TEXT("}, line: {") + tss.str() + TLOG_TEXT("}.");
	Trace(str);
}

void CTWLogger::TraceAssert(const tchar* szFile, int line, const tchar* strInfo)
{
	if((m_usWriteStatus & LEVEL_ERROR_WRITEABLE) != LEVEL_ERROR_WRITEABLE) return;
	tstring str(GetTimeOfDay() + TRACE_ASSERT);
	tstringstream tss;
	tss << line;
	str += strInfo;
	str = str + TLOG_TEXT(" file: {") + szFile + TLOG_TEXT("}, line: {") + tss.str() + TLOG_TEXT("}.");
	Trace(str);
}

void CTWLogger::Trace_format(const tchar* fmt, va_list list_arg)
{
	int n;
	n = _vsctprintf(fmt, list_arg);
//#ifdef UNICODE
//	n = _vscwprintf(fmt, list_arg);
//#else
//	n = _vscprintf(fmt, list_arg);
//#endif
	
	if(n > 0)
	{
		tchar *buf = new tchar[n+1];
		memset(buf, 0, sizeof(tchar)*(n+1));
		_vstprintf_s(buf, n+1, fmt, list_arg);
//#ifdef UNICODE
//		vswprintf_s(buf, n+1, fmt, list_arg);
//#else
//		vsprintf_s(buf, n+1, fmt, list_arg);
//#endif
		Trace(tstring(buf));
		delete buf;
		buf = NULL;
	}
	va_end(list_arg);
}

void CTWLogger::TraceInfor_f(const tchar* fmt, ...)
{
	if((m_usWriteStatus & LEVEL_INFOR_WRITEABLE) != LEVEL_INFOR_WRITEABLE) return;
	tstring str(GetTimeOfDay()+TRACE_INFOR + fmt);
	va_list arg;
	va_start(arg, fmt);
	Trace_format(str.c_str(), arg);
}

void CTWLogger::TraceDebug_f(const tchar* fmt, ...)
{
	if((m_usWriteStatus & LEVEL_DEBUG_WRITEABLE) != LEVEL_DEBUG_WRITEABLE) return;
	tstring str(GetTimeOfDay()+TRACE_DEBUG + fmt);
	va_list arg;
	va_start(arg, fmt);
	Trace_format(str.c_str(), arg);
}

void CTWLogger::TraceWarning_f(const tchar* fmt, ...)
{
	if((m_usWriteStatus & LEVEL_WARNING_WRITEABLE) != LEVEL_WARNING_WRITEABLE) return;
	tstring str(GetTimeOfDay()+TRACE_WARNNING + fmt);
	va_list arg;
	va_start(arg, fmt);
	Trace_format(str.c_str(), arg);
}

void CTWLogger::TraceError_f(const tchar* fmt, ...)
{
	if((m_usWriteStatus & LEVEL_ERROR_WRITEABLE) != LEVEL_ERROR_WRITEABLE) return;
	tstring str(GetTimeOfDay()+TRACE_ERROR + fmt);
	va_list arg;
	va_start(arg, fmt);
	Trace_format(str.c_str(), arg);
}

void CTWLogger::TraceDefault_f(const tchar* fmt, ...)
{
	if((m_usWriteStatus & LEVEL_LIMITED_WRITEABLE) != LEVEL_LIMITED_WRITEABLE) return;
	tstring str(GetTimeOfDay()+TRACE_DEFAULT + fmt);
	va_list arg;
	va_start(arg, fmt);
	Trace_format(str.c_str(), arg);
}

void CTWLogger::TraceTrace_f(const tchar* szFile, int line, const tchar* fmt, ...)
{
	if((m_usWriteStatus & LEVEL_TRACE_WRITEABLE) != LEVEL_TRACE_WRITEABLE) return;
	tstring str(GetTimeOfDay()+TRACE_TRACE + fmt);
	tstringstream tss;
	tss << line;
	//str += TLOG_TEXT("{");
	//str += fmt;
	//str += TLOG_TEXT("}");
	str = str + TLOG_TEXT(" file: {") + szFile + TLOG_TEXT("}, line: {") + tss.str() + TLOG_TEXT("}.");
	va_list arg;
	va_start(arg, fmt);
	Trace_format(str.c_str(), arg);
}


/*获取当前调用该日志文件的程序路径或程序名称（无类型后缀），主用于日志名称声明
 *@param: [out] outFilePath 获取到程序路径或程序名称的字符串
 *        [in] sizeLen outFilePath字符数组的大小
 *        [in] fetchKind FALSE表示仅获取程序路径，TRUE表示获取程序名称
 *@return 如果获取成功则表示实际得到的字符串长度，-1表示失败
 */
DWORD CTWLogger::GetCurExeNameOrPath(tchar* outFilePath, int sizeLen, int fetchKind)
{
	DWORD tmpDwRes = 0;
	if(sizeLen <= 0) 
		return -1;
	memset(outFilePath, 0, sizeof(tchar)*sizeLen);
	if(tmpDwRes = GetModuleFileName(NULL, outFilePath, sizeLen) == 0)
	{
		_stprintf_s(outFilePath, sizeLen, TLOG_TEXT("GetModuleFileName failed %d"), GetLastError());
		return -1;
	}
	if(fetchKind == MODE_DEFAULT)
	{
		return tmpDwRes;
	}

	if(fetchKind == MODE_FILENAME_WITH_PATH)
	{
		tchar* pch2 = _tcsrchr(outFilePath, TLOG_TEXT('.'));
		if(pch2 == NULL)
		{
			return -1;
		}
		pch2[0] = TLOG_TEXT('\0');
		return (int)(pch2 - outFilePath);
	}
	
	tchar* pch1 = _tcsrchr(outFilePath, TLOG_TEXT('\\'));
	if(fetchKind == MODE_DIRECTORY)
	{
		if(pch1 == NULL)
		{
			return -1;
		}
		if(int(pch1 - outFilePath) + 1 < sizeLen)
		{
			*(pch1 + 1) = TLOG_TEXT('\0');
		}
		return (int)(pch1 - outFilePath + 1);
	}

	tchar tmpFilePath[MAX_PATH] = {0};
	int idx = 0;
	tchar* pstart = pch1 + 1;
	if(fetchKind == MODE_FILENAME)
	{
		for(; *pstart != TLOG_TEXT('\0'); idx++)
		{
			tmpFilePath[idx] = *pstart++;
		}
		tmpFilePath[idx] = TLOG_TEXT('\0');
		memcpy_s(outFilePath, sizeof(tchar)*sizeLen, tmpFilePath, sizeof(tchar)*(idx+1));
		return idx+1;
	}

	if(fetchKind == MODE_FILENAME_ONLY)
	{
		tchar* pch2 = _tcsrchr(outFilePath, TLOG_TEXT('.'));
		if(pch1 == NULL || pch2 == NULL)
		{
			return -1;
		}

		for(; pstart < pch2; idx++)
		{
			tmpFilePath[idx] = *pstart++;
		}
		tmpFilePath[idx] = TLOG_TEXT('\0');
		memcpy_s(outFilePath, sizeof(tchar)*sizeLen, tmpFilePath, sizeof(tchar)*(idx+1));
		return idx+1;
	}

	return -1;
}


void CTWLogger::DelayLoop(unsigned long usec)
{
	LOG_FUNCTION();
	LARGE_INTEGER freq, start, now;
	//返回硬件支持的高精度计数器的每秒钟嘀嗒的次数，零表示硬件不支持，读取失败
	if (!QueryPerformanceFrequency(&freq))
	{
		Sleep(usec);
	}
	else
	{
		QueryPerformanceCounter(&start);
		for(;;) 
		{
			QueryPerformanceCounter((LARGE_INTEGER*) &now);
			if( ((double)(now.QuadPart - start.QuadPart) / (double)freq.QuadPart)  * 1000 > usec ) break;
		}
	}

}

bool CTWLogger::WriteToFile(tstring str)
{
	bool flag = false;
	EnterCriticalSection(&s_cs);
	try
	{
		m_logStream.open(m_fileName.c_str(), tfstream::in | tfstream::app);
		if(!m_logStream)
		{
			//MessageBox(NULL, TLOG_TEXT(m_fileName.c_str()), 
			//	TLOG_TEXT(""), MB_ICONINFORMATION | MB_OK);
			str.clear();
		}
		
		else if(m_logStream.is_open())
		{
			m_logStream << str.c_str();// << endl;
			m_logStream.close();
			flag = true;
		}

	}
	catch (std::exception e)
	{
		str.clear();
		LeaveCriticalSection(&s_cs);
		return flag;
	}

	LeaveCriticalSection(&s_cs);
	return flag;
}

bool CTWLogger::ClearToFile()
{
	bool flag = false;
	WaitForSingleObject(hMutexForBuffer, INFINITE);
	if(m_strBuffer.length() > 0)
	{
		if(m_eFileMode == Day_Seperated_Mode || m_eFileMode == Module_Seperated_Mode)
		{
			m_fileName = GetCustomTime(For_File_Type);
		}
		flag = WriteToFile(m_strBuffer);
		if(flag)
		{
			m_strBuffer = TLOG_TEXT("");
			m_bufCurLen = 0;
		}
	}
	ReleaseMutex(hMutexForBuffer);
	return flag;
}

int CTWLogger::FormatLastError(tchar* szOutMsg, unsigned int sizeLen, DWORD erroCode)
{
	if(sizeLen == 0 || szOutMsg == NULL)
		return 0;

	LPVOID lpMsgBuf;
	if(erroCode == 0)
	{
		erroCode = ::GetLastError();
	}

	if(!FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		erroCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL ))
	{
		return 0;
	}
	_tcscpy_s(szOutMsg, sizeLen,(LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return _tcslen(szOutMsg);
}

void CTWLogger::Start()
{
	QueryPerformanceCounter(&m_liPerfStart);
}

__int64 CTWLogger::Now() const
{
	LARGE_INTEGER liPerfNow;
	QueryPerformanceCounter(&liPerfNow);
	return (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
}

__int64 CTWLogger::NowInMicro() const
{
	LARGE_INTEGER liPerfNow;
	QueryPerformanceCounter(&liPerfNow);
	return (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000000) / m_liPerfFreq.QuadPart);
}

void CTWLogger::TraceErrWithLastCode(const tchar* strInfo)
{
	DWORD dwErrCode = ::GetLastError();
	TCHAR szMsg[MIN_INFOR_LEN] = {0};
	int nLen = FormatLastError(szMsg, MIN_INFOR_LEN, dwErrCode);
	TraceError_f(TLOG_TEXT("%s : (%d)%s."), strInfo, dwErrCode, szMsg);
	return;
}

const tchar * _GetFileNameForLog(const tchar *pszFilePath)
{
	if(pszFilePath == NULL || _tcslen(pszFilePath) == 0)
		return TLOG_TEXT("empty");

	const tchar* backlash = _tcsrchr(pszFilePath, (int)(TLOG_TEXT('\\')));
	if(!backlash)
	{
		return pszFilePath;
	}
	return (backlash+1);
}

#if !(defined(TWINKLE_LOGGER_VERSION) && TWINKLE_LOGGER_VERSION == 2)


unsigned int _stdcall CTWLogger::CAutoWriteHelper::TimerWriteProc(void* param)
{
	if(param == nullptr)
	{
		return -1;
	}
	CTWLogger::CAutoWriteHelper* helper = (CTWLogger::CAutoWriteHelper*)param;
	HANDLE hEvent = helper->GetLogTimerEvent();
	BOOL bLoop = TRUE;
	if(!hEvent)
	{
		return 0;
	}
	while(bLoop)
	{
		DWORD dwRet = WaitForSingleObject(hEvent, TIME_TO_WRITE_LOG);
		switch(dwRet)
		{
		case WAIT_OBJECT_0:
			//WRITE_TRACE_PARAM(TLOG_TEXT("WAIT_OBJECT_0"));
			helper->WriteToLog();
			bLoop = FALSE;
			break;
		case WAIT_TIMEOUT:
			//WRITE_TRACE_PARAM(TLOG_TEXT("WAIT_TIMEOUT"));
			helper->WriteToLog();
			break;
		case WAIT_FAILED:
			WRITE_TRACE_PARAM_INNER_USE(TLOG_TEXT("WAIT_FAILED"));
			bLoop = FALSE;
			break;
		}
	}
	//WRITE_TRACE_PARAM(TLOG_TEXT("hiahiahia"));
	return 0;
}

#else

TWLoggerShell TWLoggerShell::GetInstance(const tstring& name)
{
	return GetLoggerFactory()->GetLoggerProduct(name);
}

LoggerList TWLoggerShell::GetCurrentLoggers()
{
	LoggerList list;
	GetLoggerFactory()->InitializeLoggerList(list);
	return list;
}

bool TWLoggerShell::Exists(const tstring& name)
{
	return false;
}

TWLoggerShell TWLoggerShell::GetDefaultLogger()
{
	TWLoggerShell instance;
	GetLoggerFactory()->InitializeDefaultLogger(instance);
	return instance;
}

//Define real global variable.
LoggerFactory logger_factory;

#endif