#pragma once

#include "stdafx.h"

class BufferedWriter
{
public:
	BufferedWriter() = delete;
	BufferedWriter(HANDLE fp, pfWin32Err WinErrFunc);
	~BufferedWriter();

	BOOL append(LPCWSTR text, DWORD len);
	BOOL append(LPCWSTR format, ...);
	BOOL append(LPCWSTR format, va_list args);
	BOOL write();

private:
	const HANDLE	_fp;
	const pfWin32Err _winErrFunc;

	LPSTR	_buf;
	DWORD	_lenBytes;
	DWORD	_capacityBytes;

	BOOL ensureCapacity(DWORD cap);
	BOOL ensureAppend(DWORD lenToAppend);
};
