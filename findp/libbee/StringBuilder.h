#pragma once

class StringBuilder
{
public:
	StringBuilder(UINT codepage, DWORD initialSize, pfWin32Err WinErrFunc);
	~StringBuilder();

	BOOL append(LPCWSTR text, DWORD cchWideChar);
	BOOL appendf(LPCWSTR format, ...);
	BOOL appendv(LPCWSTR format, va_list args);
	BOOL appendByte(char byte);

	void  setByteLength(DWORD len)	{ _lenBytes = len; }
	DWORD getByteLength()			{ return _lenBytes; }
	void  reset()					{ _lenBytes = 0; }

	LPSTR getBuffer() { return _buf;  }

private:

	const pfWin32Err	_winErrFunc;
	const UINT			_codepage;

	LPSTR	_buf;
	DWORD	_lenBytes;
	DWORD	_capacityBytes;

	BOOL ensureAppend(DWORD lenToAppend);
	BOOL ensureCapacity(DWORD capacityNeeded);
};
