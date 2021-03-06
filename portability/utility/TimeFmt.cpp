#include "TimeFmt.h"
using namespace base;
#include <stdio.h>
#include <sys/time.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction & Destruction 

TimeFmt::TimeFmt()
{
}


TimeFmt::~TimeFmt()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations 

uint32_t TimeFmt::CurTimeStamp()
{
	time_t lt = time(NULL);
	localtime(&lt); 
	return (uint32_t)lt;
}


string TimeFmt::CurTime_asStr(const ETimeFmtType eOutFmt)
{
	time_t lt = time(NULL);
	struct tm *ptr = localtime(&lt);
	char sTime[128];
	sTime[0] = '\0';
	if(eOutFmt == _TIME_FMT_STD)	// "YYYY-MM-DD hh:mm:ss"
		strftime(sTime, 128, "%F %T",ptr);
	else if(eOutFmt == _TIME_FMT_NOBLANK)	// "YYYY-MM-DD_hh:mm:ss"
		strftime(sTime, 128, "%F_%T",ptr);
	else if(eOutFmt == _TIME_FMT_COMPACT)	// "YYYYMMDDhhmmss"
	{
		sprintf(sTime, "%04d%02d%02d%02d%02d%02d", ptr->tm_year+1900, ptr->tm_mon+1,
			ptr->tm_mday, ptr->tm_hour, ptr->tm_min, ptr->tm_sec);
	}
	else
		sTime[0] = '\0'; 
	return string(sTime);
}


string TimeFmt::TimeConv_Uint32ToStr(const uint32_t unTimeStamp, const ETimeFmtType eOutFmt)
{
	time_t rawtime = (time_t)unTimeStamp;
	struct tm* stTime = localtime(&rawtime);
	char buf[256];
	if(eOutFmt == _TIME_FMT_STD)
	{ // "YYYY-MM-DD hh:mm:ss"
		sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", stTime->tm_year+1900, stTime->tm_mon+1,
			stTime->tm_mday, stTime->tm_hour, stTime->tm_min, stTime->tm_sec);
	}
	else if(eOutFmt == _TIME_FMT_NOBLANK)
	{ // "YYYY-MM-DD_hh:mm:ss"
		sprintf(buf, "%04d-%02d-%02d_%02d:%02d:%02d", stTime->tm_year+1900, stTime->tm_mon+1,
			stTime->tm_mday, stTime->tm_hour, stTime->tm_min, stTime->tm_sec);
	}
	else if(eOutFmt == _TIME_FMT_COMPACT)	
	{ // "YYYYMMDDhhmmss"
		sprintf(buf, "%04d%02d%02d%02d%02d%02d", stTime->tm_year+1900, stTime->tm_mon+1,
			stTime->tm_mday, stTime->tm_hour, stTime->tm_min, stTime->tm_sec);
	}
	else
	{
		buf[0] = '\0';
	}
	return string(buf);
}


uint32_t TimeFmt::TimeConv_StrToUint32(const char* sTime, const ETimeFmtType eInFmt)
{
	time_t rawtime; 
	time(&rawtime);         
	struct tm* stTime = localtime(&rawtime);
	if(eInFmt == _TIME_FMT_STD)
	{ // "YYYY-MM-DD hh:mm:ss"
		sscanf(sTime, "%d-%d-%d %d:%d:%d", &stTime->tm_year, &stTime->tm_mon,
			&stTime->tm_mday, &stTime->tm_hour, &stTime->tm_min, &stTime->tm_sec);
	}
	else if(eInFmt == _TIME_FMT_NOBLANK)
	{ // "YYYY-MM-DD_hh:mm:ss"
		sscanf(sTime, "%d-%d-%d_%d:%d:%d", &stTime->tm_year, &stTime->tm_mon,
			&stTime->tm_mday, &stTime->tm_hour, &stTime->tm_min, &stTime->tm_sec);
	}
	else
	{
		return 0;
	}

	stTime->tm_year -= 1900;        
	stTime->tm_mon -= 1;

	time_t ttTime = mktime(stTime); 
	return (uint32_t)ttTime;
}


string TimeFmt::DateConv_Uint32ToStr(const uint32_t unTimeStamp, const EDateFmtType eOutFmt)
{
	time_t rawtime = (time_t)unTimeStamp;
	struct tm* stTime = localtime(&rawtime);  
	char buf[256];
	if(eOutFmt == _DATE_FMT_STD) // "YYYY-MM-DD"
		sprintf(buf, "%04d-%02d-%02d", stTime->tm_year+1900, stTime->tm_mon+1, stTime->tm_mday);
	else if(eOutFmt == _DATE_FMT_COMPACT)	// "YYYYMMDD"
		sprintf(buf, "%04d%02d%02d", stTime->tm_year+1900, stTime->tm_mon+1, stTime->tm_mday);
	else
		buf[0] = '\0'; 
	return string(buf);
}	


uint32_t TimeFmt::DateConv_StrToUint32(const char* sDate)
{
	time_t rawtime;
	time(&rawtime);
	struct tm* stTime = localtime(&rawtime);
	sscanf(sDate, "%d-%d-%d", &stTime->tm_year, &stTime->tm_mon, &stTime->tm_mday);

	stTime->tm_hour = 0; 
	stTime->tm_min = 0;
	stTime->tm_sec = 0; 

	stTime->tm_year -= 1900;
	stTime->tm_mon -= 1;     

	time_t ttTime = mktime(stTime); 
	return (uint32_t)ttTime;  
}



