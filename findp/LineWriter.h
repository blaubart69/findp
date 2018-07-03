#pragma once

class LineWriter
{
public:
	LineWriter() = delete;
	LineWriter(HANDLE filehandle, UINT codepage, DWORD initialSize, pfWin32Err WinErrFunc);
	~LineWriter();

	BOOL append(LPCWSTR text, DWORD cchWideChar);
	BOOL appendf(LPCWSTR format, ...);
	BOOL appendUTF8BOM();

	void setLength(WORD len)	{ _lenBytes = len;   }
	DWORD getLength()			{ return _lenBytes;  }
	void reset()				{ _lenBytes = 0;	 }

	BOOL write();
	BOOL writeAndReset();

private:

	const pfWin32Err	_winErrFunc;
	const HANDLE 		_filehandle;
	const UINT			_codepage;

	LPSTR	_buf;
	DWORD	_lenBytes;
	DWORD	_capacityBytes;

	BOOL internal_write();
	BOOL appendv(LPCWSTR format, va_list args);
	BOOL ensureAppend(DWORD lenToAppend);
	BOOL ensureCapacity(DWORD capacityNeeded);
};
