#include "stdafx.h"

StringBuilder::StringBuilder(UINT codepage, DWORD initialSize, pfWin32Err WinErrFunc)
	: _winErrFunc(WinErrFunc)
	, _codepage(codepage)
{
	_lenBytes = 0;
	_capacityBytes = initialSize;
	_buf = (LPSTR)HeapAlloc(GetProcessHeap(), 0, _capacityBytes);

	if (_buf == nullptr)
	{
		if (_winErrFunc) _winErrFunc(L"HeapAlloc", L"StringBuilder::ctor");
	}
}

StringBuilder::~StringBuilder()
{
	if (_buf != nullptr)
	{
		HeapFree(GetProcessHeap(), 0, _buf);
	}
}

BOOL StringBuilder::append(LPCWSTR text, DWORD cchWideChar)
{
	ensureAppend(cchWideChar * 4); // worst case: 4 bytes per character

	const DWORD cbBytesLeft = _capacityBytes - _lenBytes;
	const LPSTR StartPositionInBuffer = _buf + _lenBytes;

	const int bytesWritten = WideCharToMultiByte(
		_codepage
		, 0								// dwFlags [in]
		, text							// lpWideCharStr [in]
		, cchWideChar					// cchWideChar [in]
		, StartPositionInBuffer			// lpMultiByteStr [out, optional]
		, cbBytesLeft					// cbMultiByte [in]
		, NULL							// lpDefaultChar[in, optional]
		, NULL);						// lpUsedDefaultChar[out, optional]

	BOOL ok;
	if (bytesWritten == 0)
	{
		ok = FALSE;
		if (_winErrFunc)
		{
			WCHAR buf[1024];
			wsprintfW(buf, L"StringBuilder: codepage [%u], cchWideChar [%u], _lenBytes [%u], _capBytes [%u], bytesLeft [%u], text [%s]\n", _codepage, cchWideChar, _lenBytes, _capacityBytes, cbBytesLeft, text);
			_winErrFunc(L"WideCharToMultiByte", buf);
		}
	}
	else
	{
		ok = TRUE;
		_lenBytes += bytesWritten;
	}

	return ok;
}

BOOL StringBuilder::appendf(LPCWSTR format, ...)
{
	va_list args;
	va_start(args, format);
	BOOL rc = appendv(format, args);
	va_end(args);

	return rc;
}

BOOL StringBuilder::appendv(LPCWSTR format, va_list args)
{
	WCHAR buffer[1024];
	const int writtenChars = wvsprintfW(buffer, format, args);
	return append(buffer, writtenChars);
}

BOOL StringBuilder::appendByte(char byte)
{
	BOOL ok = ensureAppend(1);
	if (ok)
	{
		_buf[_lenBytes++] = byte;
	}
	return ok;
}

BOOL StringBuilder::ensureAppend(DWORD bytesToAppend)
{
	DWORD bytesLeft = _capacityBytes - _lenBytes;

	if (bytesLeft >= bytesToAppend)
	{
		return TRUE;
	}

	return ensureCapacity(bytesLeft + bytesToAppend);
}

BOOL StringBuilder::ensureCapacity(DWORD bytesCapacityNeeded)
{
	BOOL ok = TRUE;

	if (bytesCapacityNeeded > _capacityBytes)
	{
		DWORD newCapBytes = _capacityBytes * 4;
		DWORD diffBytes = bytesCapacityNeeded - _capacityBytes;

		newCapBytes = max(newCapBytes, diffBytes);

		_buf = (LPSTR)HeapReAlloc(
			GetProcessHeap()
			, 0
			, _buf
			, newCapBytes);

		if (_buf == NULL)
		{
			if (_winErrFunc) _winErrFunc(L"HeapReAlloc", L"StringBuilder::ensureCapacity(DWORD)");
			ok = FALSE;
		}
		else
		{
			_capacityBytes = newCapBytes;
		}
	}

	return ok;
}
