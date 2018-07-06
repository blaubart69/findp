#include "stdafx.h"

Log* Log::_instance = nullptr;

Log::Log (int level)
	: _level(level)
{
	_lineWriter = new LineWriter(
		  GetStdHandle(STD_ERROR_HANDLE)
		, GetConsoleOutputCP()
		, 1024
		, Log::win32errfunc);
}

void Log::writeLogLine(WCHAR prefix, const WCHAR* format, va_list args)
{
	_lineWriter->reset();

	switch (prefix)
	{
	case L'D': _lineWriter->append(L"D: ", 3); break;
	case L'W': _lineWriter->append(L"W: ", 3); break;
	case L'I': _lineWriter->append(L"I: ", 3); break;
	case L'E': _lineWriter->append(L"E: ", 3); break;
	default:   _lineWriter->append(L"?: ", 3); break;
	}

	_lineWriter->appendv(format, args);
	_lineWriter->append(L"\r\n", 2);
	_lineWriter->write();
}


void Log::dbg(const WCHAR* format, ...) 
{
	if (_level < 3) return;

	va_list args;
	va_start(args, format);
	writeLogLine(L'D', format, args);
	va_end(args);
}
void Log::inf(const WCHAR* format, ...) 
{
	if (_level < 2) return;
	va_list args;
	va_start(args, format);
	writeLogLine(L'I', format, args);
	va_end(args);
}
void Log::wrn(const WCHAR* format, ...) 
{
	if (_level < 1) return;
	va_list args;
	va_start(args, format);
	writeLogLine(L'W', format, args);
	va_end(args);
}
void Log::err(const WCHAR* format, ...) 
{
	va_list args;
	va_start(args, format);
	writeLogLine(L'E', format, args);
	va_end(args);
}

void Log::write(const WCHAR * format, ...)
{
	va_list args;
	va_start(args, format);
	_lineWriter->reset();
	_lineWriter->appendv(format, args);
	_lineWriter->write();
	va_end(args);
}

void Log::writeLine(const WCHAR * format, ...)
{
	va_list args;
	va_start(args, format);
	_lineWriter->reset();
	_lineWriter->appendv(format, args);
	_lineWriter->append(L"\r\n", 2);
	_lineWriter->write();
	va_end(args);
}

void Log::resetBuffer()
{
	_lineWriter->reset();
}

void Log::append(LPCWSTR text, DWORD cchWideChar)
{
	_lineWriter->append(text, cchWideChar);
}

void Log::appendf(const WCHAR* format, ...)
{
	va_list args;
	va_start(args, format);
	_lineWriter->appendv(format, args);
	va_end(args);
}

void Log::writeBuffer()
{
	_lineWriter->write();
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
		_lineWriter->reset();
		_lineWriter->appendf(L"E-Win32API: Lasterror: 0x%x, Api: %ls\n", GetLastError(), L"FormatMessage");
		_lineWriter->write();
		lpWindowsErrorText = L"!!! no error message available since FormatMessage failed !!!";
	}

	_lineWriter->reset();
	_lineWriter->appendf(L"E-Win32API: Lasterror: 0X%X, Api: %ls, Msg: %ls, param: %ls\n", LastErr, Apiname, lpWindowsErrorText, param);
	_lineWriter->write();

	if (rcFormatMsg == 0)
	{
		LocalFree((HLOCAL)lpWindowsErrorText);
	}
}

void Log::win32errfunc(LPCWSTR Apiname, LPCWSTR param)
{
	Log::Instance()->win32err(Apiname, param);
}

