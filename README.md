TWLogger
========
A common logger implemented by C++ on the Windows platform

Update history
==============
  @date:2016-6-26 set option for no buffer or not.
  
	@date:2016-7-2	delete erroCode stored; 
  
					        add Trace CTWLogger format; add GetLastError() formatting message;
                  
					        add LOG_FUNCION() to log function enter and leave event;
                  
	@date:2016-7-4	improve the logger time accurating to millisecond;
  
  @date:2016-7-26 change the time-formate depend on file_mode.
  
	@date:2016-7-29 support for unicode characterset.
  
	@date:2016-7-29	write the log buffer into the file for specified time interval.
  
	@date:2016-9-2	add 'TRACE_FUNCTION' to log function with returned value(type: int, dword).
  
  @date:2016-12-2 add header file: <tchar.h>
  
  @date:2017-1-24 add WRITE_ERRCODE_LOG
  
  @date:2017-2-9  add FileModeEnum::CustomDir_DaySplit_Mode
  
  @date:2017-5-2  add WRITE_TRACE_LOG
  
	@date:2017-5-8  add LOG_ASSERT and change WRITE_TRACE_PARAM implemention.
  
  @date:2017-5-18 add LAZY_MODEL definition for avoiding creating LogFiles folder.
  
 ----------------------------------------------------------------------------------------------
 
  @version: 2.0.0  #date: 2017 - 6 - 29

  @date:2017-7-12  support creating seperated logger instance representing its own record folder
  
  @date:2017-7-14  fix some bugs
  
  @date:2017-10-11 fix the bugs : the order of declaration for static - type m_strLogDirOrFilePath
                 would make the set - operation for m_strLogDirOrFilePath in  declaration routine
                 for CAutoWriteHelper no sense.
                 
  @date:2018-4-3   change the class name from Logger to CTWLogger

  @date:2018-4-10  Add 'EnableConsole' function to support console debug just for seeking instantly.

                 Fix some bug for Unicode char format.
