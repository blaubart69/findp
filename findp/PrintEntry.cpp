#include "stdafx.h"

BOOL ConvertFiletimeToLocalTime(const FILETIME *filetime, SYSTEMTIME *localTime);
void PrintFullEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, LineWriter *outputLine, bool printOwner, LPCWSTR owner);

void PrintEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, LineWriter *outputLine, bool printFull, bool printOwner)
{
	WCHAR owner[128];

	if (printOwner)
	{
		WCHAR filename[256];

		wsprintfW(filename, L"%s\\%s", FullBaseDir->str, finddata->cFileName);
		if (!GetOwner(filename, owner, sizeof(owner)))
		{
			StrCpyW(owner, L"n/a");
		}
	}

	if (printFull)
	{
		PrintFullEntry(FullBaseDir, finddata, outputLine, printOwner, owner);
	}
	else
	{
		DWORD lastLength = outputLine->getByteLength();
		outputLine->appendf(L"\\%s\r\n", finddata->cFileName);
		outputLine->writeBuffer_keepBuffer();
		outputLine->setByteLength(lastLength);
	}
}

void PrintFullEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, LineWriter *outputLine, bool printOwner, LPCWSTR owner)
{
	SYSTEMTIME localTime;

	outputLine->reset();
	if (ConvertFiletimeToLocalTime(&finddata->ftLastWriteTime, &localTime))
	{
		DWORD attrs = finddata->dwFileAttributes;

		ULARGE_INTEGER li;
		li.HighPart = finddata->nFileSizeHigh;
		li.LowPart  = finddata->nFileSizeLow;

		outputLine->appendf(
			L"%04u-%02u-%02u %02u:%02u:%02u"
			L"\t%c%c%c%c%c%c%c"
			L"\t%12I64u"
			L"\t",
			  localTime.wYear, localTime.wMonth, localTime.wDay
			, localTime.wHour, localTime.wMinute, localTime.wSecond
			, ((attrs & FILE_ATTRIBUTE_ARCHIVE)    != 0) ? L'A' : L'-'
			, ((attrs & FILE_ATTRIBUTE_SYSTEM)     != 0) ? L'S' : L'-'
			, ((attrs & FILE_ATTRIBUTE_HIDDEN)     != 0) ? L'H' : L'-'
			, ((attrs & FILE_ATTRIBUTE_READONLY)   != 0) ? L'R' : L'-'
			, ((attrs & FILE_ATTRIBUTE_DIRECTORY)  != 0) ? L'D' : L'-'
			, ((attrs & FILE_ATTRIBUTE_ENCRYPTED)  != 0) ? L'E' : L'-'
			, ((attrs & FILE_ATTRIBUTE_COMPRESSED) != 0) ? L'C' : L'-'
			, li.QuadPart);
	}

	if (printOwner)
	{
		outputLine->appendf(L"%s\t", owner);
	}

	outputLine->append(FullBaseDir->str, FullBaseDir->len);
	outputLine->appendf(L"\\%s\r\n", finddata->cFileName);
	outputLine->writeBuffer();
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