#include "stdafx.h"

Log* Log::_instance = nullptr;

Log::Log (int level, HANDLE outHandle, UINT codepage)
	: _level(level)
	, _outHandle(outHandle)
	, _codepage(codepage)
{
}

void Log::dbg(const WCHAR* format, ...) 
{
	if (_level < 3) return;

	va_list args;
	va_start(args, format);
	writeLogLine(L'D', format, args, true);
	va_end(args);
}
void Log::inf(const WCHAR* format, ...) 
{
	if (_level < 2) return;
	va_list args;
	va_start(args, format);
	writeLogLine(L'I', format, args, true);
	va_end(args);
}
void Log::wrn(const WCHAR* format, ...) 
{
	if (_level < 1) return;
	va_list args;
	va_start(args, format);
	writeLogLine(L'W', format, args, true);
	va_end(args);
}
void Log::err(const WCHAR* format, ...) 
{
	va_list args;
	va_start(args, format);
	writeLogLine(L'E', format, args, true);
	va_end(args);
}

void Log::write(const WCHAR * format, ...)
{
	va_list args;
	va_start(args, format);
	writeLogLine(format, args, false);
	va_end(args);
}

void Log::writeLine(const WCHAR * format, ...)
{
	va_list args;
	va_start(args, format);
	writeLogLine(format, args, true);
	va_end(args);
}

void Log::win32err(LPCWSTR Apiname) 
{
	win32err(Apiname, L"");
}

void Log::win32err(LPCWSTR Apiname, LPCWSTR param) 
{
	const DWORD LastErr = GetLastError();

	LPCWSTR lpWindowsErrorText = L"n/a";
	DWORD rcFormatMsg;
	if ((rcFormatMsg = FormatMessageW(
		  FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_MAX_WIDTH_MASK
		, NULL
		, LastErr
		, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
		, (LPWSTR)&lpWindowsErrorText
		, 0
		, NULL)) == 0)
	{
		writeLine(L"E-Win32API: Lasterror: 0x%x, Api: %ls", GetLastError(), L"FormatMessage");
		lpWindowsErrorText = L"!!! no error message available since FormatMessage failed !!!";
	}

	writeLine(L"E-Win32API: Lasterror: 0X%X, Api: %ls, Msg: %ls, param: %ls", LastErr, Apiname, lpWindowsErrorText, param);

	if (rcFormatMsg == 0)
	{
		LocalFree((HLOCAL)lpWindowsErrorText);
	}
}

void Log::win32errfunc(LPCWSTR Apiname, LPCWSTR param)
{
	Log::Instance()->win32err(Apiname, param);
}
//
// ----- PRIVATE -----
//
void Log::writeLogLine(const WCHAR* format, va_list args, bool appendNewLine)
{
	writeLogLine(L'\0', format, args, appendNewLine);
}
void Log::writeLogLine(WCHAR prefix, const WCHAR* format, va_list args, bool appendNewLine)
{
	WCHAR buf[1024];
	int prefixLen;

	if (prefix == L'\0')
	{
		prefixLen = 0;
	}
	else
	{
		prefixLen = 3;
		buf[0] = prefix;
		buf[1] = L':';
		buf[2] = L' ';
	}

	const int writtenChars = wvsprintfW(prefixLen + buf, format, args);

	int suffixLen;
	if (appendNewLine)
	{
		suffixLen = 2;
		buf[prefixLen + writtenChars + 0] = L'\r';
		buf[prefixLen + writtenChars + 1] = L'\n';
	}
	else
	{
		suffixLen = 0;
	}

	writeTextCodepage(buf, prefixLen + writtenChars + suffixLen);
}

BOOL Log::writeTextCodepage(const WCHAR* text, const DWORD cchWideChar)
{
	CHAR writeBuffer[2048];

	int numberMultiBytes = convertToMultiByte(_codepage, text, cchWideChar, writeBuffer, sizeof(writeBuffer));
	if (numberMultiBytes == 0)
	{
		this->win32err(L"WideCharToMultiByte", L"Log::writeTextCodepage(convertToMultiByte)");
		return FALSE;
	}

	DWORD numberBytesWritten;
	BOOL ok = WriteFile(
		_outHandle
		, writeBuffer
		, numberMultiBytes
		, &numberBytesWritten
		, NULL);

	if (!ok)
	{
		this->win32err(L"WriteFile", L"Log::writeTextCodepage(WriteFile)");
	}

	return ok;
}



