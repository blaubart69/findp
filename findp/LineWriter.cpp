#include "stdafx.h"

LineWriter::LineWriter(HANDLE filehandle, UINT codepage, DWORD initialSize, pfWin32Err WinErrFunc)
	:		_filehandle(filehandle)
		,	_winErrFunc(WinErrFunc)
		,	_codepage(codepage)
{
	_lenBytes = 0;
	_capacityBytes = initialSize;
	_buf = (LPSTR)HeapAlloc(GetProcessHeap(), 0, _capacityBytes);

	if (_buf == nullptr)
	{
		if (_winErrFunc) _winErrFunc(L"HeapAlloc", L"LineWriter::ctor");
	}
}

LineWriter::~LineWriter()
{
	if (_buf != nullptr)
	{
		HeapFree(GetProcessHeap(), 0, _buf);
	}
}

BOOL LineWriter::append(LPCWSTR text, DWORD cchWideChar)
{
	ensureAppend(cchWideChar * 4); // worst case: 4 bytes per character

	const DWORD cbBytesLeft				= _capacityBytes - _lenBytes;
	const LPSTR StartPositionInBuffer	= _buf + _lenBytes;

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
	if ( bytesWritten == 0 )
	{
		ok = FALSE;
		if (_winErrFunc)
		{
			WCHAR buf[1024];
			wsprintfW(buf, L"LineWriter: codepage [%u], cchWideChar [%u], _lenBytes [%u], _capBytes [%u], bytesLeft [%u], text [%s]\n", _codepage, cchWideChar, _lenBytes, _capacityBytes, cbBytesLeft, text);
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

BOOL LineWriter::appendf(LPCWSTR format, ...)
{
	va_list args;
	va_start(args, format);
	BOOL rc = appendv(format, args);
	va_end(args);

	return rc;
}

BOOL LineWriter::appendUTF8BOM()
{
	BOOL ok = ensureAppend(3);

	if (ok)
	{
		_buf[_lenBytes++] = 0xEF;
		_buf[_lenBytes++] = 0xBB;
		_buf[_lenBytes++] = 0xBF;
	}

	return ok;
}

BOOL LineWriter::appendv(LPCWSTR format, va_list args)
{
	WCHAR buffer[1024];
	const int writtenChars = wvsprintfW(buffer, format, args);
	return append(buffer, writtenChars);
}

BOOL LineWriter::writeAndReset()
{
	BOOL rc = internal_write();
	this->reset();
	return rc;
}

BOOL LineWriter::write()
{
	return internal_write();
}

BOOL LineWriter::internal_write()
{
	DWORD written;
	BOOL ok = WriteFile(
		_filehandle
		, _buf
		, _lenBytes
		, &written
		, NULL);

	if (!ok)
	{
		if (_winErrFunc)
		{
			_winErrFunc(L"WriteFile", L"LineWriter::write()");
		}
	}

	return ok;
}

BOOL LineWriter::ensureAppend(DWORD bytesToAppend)
{
	DWORD bytesLeft = _capacityBytes - _lenBytes;

	if ( bytesLeft >= bytesToAppend )
	{
		return TRUE;
	}

	return ensureCapacity(bytesLeft + bytesToAppend);
}

BOOL LineWriter::ensureCapacity(DWORD bytesCapacityNeeded)
{
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
			if (_winErrFunc) _winErrFunc(L"HeapReAlloc", L"LineWriter::ensureCapacity(DWORD)");
			return FALSE;
		}

		_capacityBytes = newCapBytes;
	}

	return TRUE;
}

