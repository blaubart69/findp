#include "stdafx.h"

BufferedWriter::BufferedWriter(HANDLE filehandle, pfWin32Err WinErrFunc)
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

BufferedWriter::~BufferedWriter()
{
	if (_buf != NULL)
	{
		HeapFree(GetProcessHeap(), 0, _buf);
	}
}

BOOL BufferedWriter::append(LPCWSTR text, DWORD len)
{
	BOOL rc = TRUE;

	ensureAppend(len * 4);

	int bytesWritten = WideCharToUTF8(
		text
		, len
		, _buf + len
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

BOOL BufferedWriter::append(LPCWSTR format, va_list args)
{
	WCHAR buffer[1024];
	int writtenChars = wvsprintfW(buffer, format, args);
	return append(buffer, writtenChars);
}

BOOL BufferedWriter::append(LPCWSTR format, ...)
{
	va_list args;
	va_start(args, format);
	BOOL rc = append(format, args);
	va_end(args);

	return rc;
}

BOOL BufferedWriter::write()
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

BOOL BufferedWriter::ensureCapacity(DWORD cap)
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

BOOL BufferedWriter::ensureAppend(DWORD lenToAppend)
{
	return ensureCapacity(_capacityBytes + lenToAppend);
}
