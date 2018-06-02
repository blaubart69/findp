#include "stdafx.h"
#include "Log.h"

Log* Log::_instance = nullptr;

void bee_vprintf(LPCWSTR prefix, const WCHAR* format, va_list args)
{
	WCHAR buffer[1024];

	int prefixlen = prefix == NULL ? 0 : lstrlen(prefix);
	if (prefixlen > 0)
	{
		lstrcpy(buffer, prefix);
	}

	int written = wvsprintfW(buffer + prefixlen, format, args);
	lstrcpy(buffer + prefixlen + written, L"\n");

	int lenToPrint = prefixlen + written + 1;

	DWORD charsWritten;
	//WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, lenToPrint, &charsWritten, NULL);
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer, lenToPrint, &charsWritten, NULL);
}

void bee_printf(LPCWSTR prefix, const WCHAR* format, ...)
{
	va_list args;
	va_start(args, format);
	bee_vprintf(prefix, format, args);
	va_end(args);
}

void Log::inf(const WCHAR* format, ...) const
{
	va_list args;
	va_start(args, format);
	bee_vprintf(L"I: ", format, args);
	va_end(args);
}
void Log::err(const WCHAR* format, ...) const
{
	va_list args;
	va_start(args, format);
	bee_vprintf(L"E: ", format, args);
	va_end(args);
}

void Log::out(const WCHAR * format, ...) const
{
	va_list args;
	va_start(args, format);
	bee_vprintf(NULL, format, args);
	va_end(args);
}

void Log::win32err(LPCWSTR Apiname) const
{
	const DWORD LastErr = GetLastError();

	const WCHAR *lpWindowsErrorText = L"n/a";
	DWORD rcFormatMsg;
	if ((rcFormatMsg = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM
		, NULL
		, LastErr
		, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
		, (LPWSTR)&lpWindowsErrorText
		, 0
		, NULL)) == 0)
	{
		bee_printf(L"E-Win32API: ", L"Lasterror: 0x%x, Api: %ls\n", GetLastError(), L"FormatMessage");
		lpWindowsErrorText = L"!!! no error message available since FormatMessage failed !!!";
	}
	bee_printf(L"E-Win32API: ", L"Lasterror: 0x%X, Api: %ls, Msg: %ls\n", LastErr, Apiname, lpWindowsErrorText);

	if (rcFormatMsg == 0)
	{
		LocalFree((HLOCAL)lpWindowsErrorText);
	}
}

