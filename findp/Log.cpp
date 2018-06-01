#include "stdafx.h"
#include "Log.h"

Log* Log::_instance = nullptr;

void bee_printf(const WCHAR* format, ...)
{
	WCHAR buffer[1024];

	va_list args;
	va_start(args, format);
	wvsprintfW(buffer, format, args);

	DWORD charsWritten;
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer, lstrlenW(buffer), &charsWritten, NULL);

	va_end(args);
}

void bee_vprintf(LPCWSTR prefix, const WCHAR* format, va_list args)
{
	WCHAR buffer[1024];

	lstrcpy(buffer, prefix);

	wvsprintfW(buffer + lstrlen(prefix), format, args);

	DWORD charsWritten;
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer, lstrlenW(buffer), &charsWritten, NULL);
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\n", 1, &charsWritten, NULL);
}

void Log::inf(const WCHAR* format, ...) const
{
	va_list args;
	va_start(args, format);

	bee_vprintf(L"I: ", format, args);

	va_end(args);
}

//Log* Log::Instance()
//{
//	if (_instance == nullptr)
//	{
//		_instance = new Log;
//	}
//	return _instance;
//}
