#include "stdafx.h"

BOOL ConvertFiletimeToLocalTime(const FILETIME *filetime, SYSTEMTIME *localTime);
void PrintFullEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, LineWriter *lineWriter);

void PrintEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, LineWriter *lineWriter, bool printFull)
{
	if (printFull)
	{
		PrintFullEntry(FullBaseDir, finddata, lineWriter);
	}
	else
	{
		DWORD lastLength = lineWriter->getLength();
		lineWriter->appendf(L"\\%s\r\n", finddata->cFileName);
		lineWriter->write();
		lineWriter->setLength(lastLength);
	}
}

void PrintFullEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, LineWriter *lineWriter)
{
	SYSTEMTIME localTime;

	lineWriter->reset();
	if (ConvertFiletimeToLocalTime(&finddata->ftLastWriteTime, &localTime))
	{
		DWORD attrs = finddata->dwFileAttributes;

		ULARGE_INTEGER li;
		li.HighPart = finddata->nFileSizeHigh;
		li.LowPart  = finddata->nFileSizeLow;

		lineWriter->appendf(
			L"%04u-%02u-%02u %02u:%02u:%02u"
			L"\t%c%c%c%c%c"
			L"\t%12I64u"
			L"\t",
			localTime.wYear, localTime.wMonth, localTime.wDay
			, localTime.wHour, localTime.wMinute, localTime.wSecond
			, ((attrs & FILE_ATTRIBUTE_ARCHIVE)   != 0) ? L'A' : L'-'
			, ((attrs & FILE_ATTRIBUTE_SYSTEM)    != 0) ? L'S' : L'-'
			, ((attrs & FILE_ATTRIBUTE_HIDDEN)    != 0) ? L'H' : L'-'
			, ((attrs & FILE_ATTRIBUTE_READONLY)  != 0) ? L'R' : L'-'
			, ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0) ? L'D' : L'-'
			, li.QuadPart);
	}

	lineWriter->append(FullBaseDir->str, FullBaseDir->len);
	lineWriter->appendf(L"\\%s\r\n", finddata->cFileName);
	lineWriter->write();
}

BOOL ConvertFiletimeToLocalTime(const FILETIME *filetime, SYSTEMTIME *localTime)
{
	SYSTEMTIME UTCSysTime;
	if ( !FileTimeToSystemTime(filetime, &UTCSysTime) )
	{
		Log::Instance()->win32err(L"FileTimeToSystemTime");
		return FALSE;
	}

	if ( !SystemTimeToTzSpecificLocalTime(NULL, &UTCSysTime, localTime))
	{
		Log::Instance()->win32err(L"FileTimeToSystemTime");
		return FALSE;
	}

	return TRUE;
}