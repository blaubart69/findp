#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT

#include <Windows.h>

#include "beewstring.h"
#include "LastError.h"

bee::LastError& GetOwner(LPCWSTR filename, bee::wstring* owner, bee::LastError* lastErr);

bee::LastError& ConvertFiletimeToLocalTime(const FILETIME* filetime, SYSTEMTIME* localTime, bee::LastError* lastErr)
{
	SYSTEMTIME UTCSysTime;
	if (!FileTimeToSystemTime(filetime, &UTCSysTime))
	{
		lastErr->set("FileTimeToSystemTime");
	}
	else if (!SystemTimeToTzSpecificLocalTime(NULL, &UTCSysTime, localTime))
	{
		lastErr->set("FileTimeToSystemTime");
	}

	return *lastErr;
}

void Append_Time_Attributes_Size(nt::FILE_DIRECTORY_INFORMATION* finddata, bee::wstring* outBuffer, bee::LastError* lastErr)
{
	SYSTEMTIME localTime;

	if (ConvertFiletimeToLocalTime((FILETIME*)&(finddata->LastWriteTime), &localTime, lastErr).failed())
	{
		//lastErr->print();
	}
	else
	{
		DWORD attrs = finddata->FileAttributes;

		outBuffer->appendf(
			L"%04u-%02u-%02u %02u:%02u:%02u"
			L"\t%c%c%c%c%c%c%c"
			L"\t%12I64u"
			L"\t",
			  localTime.wYear, localTime.wMonth,  localTime.wDay
			, localTime.wHour, localTime.wMinute, localTime.wSecond
			, ((attrs & FILE_ATTRIBUTE_ARCHIVE)    != 0) ? L'A' : L'-'
			, ((attrs & FILE_ATTRIBUTE_SYSTEM)     != 0) ? L'S' : L'-'
			, ((attrs & FILE_ATTRIBUTE_HIDDEN)     != 0) ? L'H' : L'-'
			, ((attrs & FILE_ATTRIBUTE_READONLY)   != 0) ? L'R' : L'-'
			, ((attrs & FILE_ATTRIBUTE_DIRECTORY)  != 0) ? L'D' : L'-'
			, ((attrs & FILE_ATTRIBUTE_ENCRYPTED)  != 0) ? L'E' : L'-'
			, ((attrs & FILE_ATTRIBUTE_COMPRESSED) != 0) ? L'C' : L'-'
			, finddata->EndOfFile.QuadPart);
	}
}

bee::LastError& PrintEntry(const bee::wstring& FullBaseDir, nt::FILE_DIRECTORY_INFORMATION* finddata, bee::wstring* outBuffer, bool printFull, bool printOwner, bool printQuoted, bee::LastError* lastErr)
{
	if (printFull)
	{
		Append_Time_Attributes_Size(finddata, outBuffer, lastErr);

		if (printOwner)
		{
			bee::wstring owner;

			bee::wstring tmpFullfilename;
			tmpFullfilename.assign(FullBaseDir);
			tmpFullfilename.push_back(L'\\');
			tmpFullfilename.append(finddata->FileName, finddata->FileNameLength / sizeof(WCHAR));

			if (GetOwner(tmpFullfilename.c_str(), &owner, lastErr).failed() )
			{
				owner.assign(L"n/a");
			}

			outBuffer->push_back(L'\t');
			outBuffer->append(owner);
			outBuffer->push_back(L'\t');
		}
	}

	static bee::wstring CRLF(L"\r\n");

	outBuffer->append(FullBaseDir);
	outBuffer->push_back(L'\\');
	outBuffer->append(finddata->FileName, finddata->FileNameLength / sizeof(WCHAR));
	outBuffer->append(CRLF);

	return *lastErr;
}


