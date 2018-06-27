#include "stdafx.h"

UTF8Writer::UTF8Writer(HANDLE filehandle, pfWin32Err WinErrFunc)
	: _fp(filehandle), _winErrFunc()
{
	_lenBytes = 0;
	_capacityBytes = 4096;
	_buf = (LPSTR)HeapAlloc(GetProcessHeap(), 0, _capacityBytes);

	if (_buf == NULL)
	{
		if (_winErrFunc) _winErrFunc(L"HeapAlloc", L"BufferedWriter::ctor");
	}
}

UTF8Writer::~UTF8Writer()
{
	if (_buf != NULL)
	{
		HeapFree(GetProcessHeap(), 0, _buf);
	}
}

BOOL UTF8Writer::append(LPCWSTR text, DWORD cchWideChar)
{
	BOOL rc = TRUE;

	ensureAppend(cchWideChar * 2 * 4);

	int bytesWritten = WideCharToUTF8(
		text
		, cchWideChar
		, _buf + _lenBytes
		, _capacityBytes - _lenBytes);

	if (bytesWritten == 0)
	{
		rc = FALSE;
		if (_winErrFunc) _winErrFunc(L"WideCharToMultiByte", text);
	}
	else
	{
		_lenBytes += bytesWritten;
	}

	return rc;
}

BOOL UTF8Writer::append(LPCWSTR format, va_list args)
{
	WCHAR buffer[1024];
	int writtenChars = wvsprintfW(buffer, format, args);
	return append(buffer, writtenChars);
}

BOOL UTF8Writer::append(LPCWSTR format, ...)
{
	va_list args;
	va_start(args, format);
	BOOL rc = append(format, args);
	va_end(args);

	return rc;
}

BOOL UTF8Writer::write()
{
	BOOL ok = WriteFile(
		_fp
		, _buf
		, _lenBytes
		, NULL
		, NULL);
	
	if (ok)
	{
		_lenBytes = 0;
	}
	else
	{
		if (_winErrFunc) _winErrFunc(L"WriteFile", L"BufferedWriter::append()");
	}

	return ok;
}

BOOL UTF8Writer::ensureCapacity(DWORD cap)
{
	if (cap > _capacityBytes)
	{
		DWORD newCapBytes = _capacityBytes * 4;
		DWORD diffBytes = cap - _capacityBytes;

		newCapBytes = max(newCapBytes, diffBytes);

		_buf = (LPSTR)HeapReAlloc(
			GetProcessHeap()
			, 0
			, _buf
			, newCapBytes);

		if (_buf == NULL)
		{
			if (_winErrFunc) _winErrFunc(L"HeapReAlloc", L"BufferedWriter::ensureCapacity(DWORD)");
			return FALSE;
		}

		_capacityBytes = newCapBytes;
	}

	return TRUE;
}

BOOL UTF8Writer::ensureAppend(DWORD lenToAppend)
{
	return ensureCapacity(_capacityBytes + lenToAppend);
}
