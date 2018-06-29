#pragma once
#include "stdafx.h"

class LineWriter
{
public:
	LineWriter() = delete;
	LineWriter(HANDLE filehandle, UINT codepage, DWORD initialSize, pfWin32Err WinErrFunc);
	~LineWriter();

	BOOL append(LPCWSTR text, DWORD cchWideChar);
	BOOL appendf(LPCWSTR format, ...);
	BOOL appendv(LPCWSTR format, va_list args);
	BOOL write();

private:

	const pfWin32Err	_winErrFunc;
	const HANDLE 		_filehandle;
	const UINT			_codepage;

	LPSTR	_buf;
	DWORD	_lenBytes;
	DWORD	_capacityBytes;

	BOOL ensureAppend(DWORD lenToAppend);
	BOOL ensureCapacity(DWORD capacityNeeded);
};
