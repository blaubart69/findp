#pragma once

#include "stdafx.h"

class UTF8Writer
{
public:
	UTF8Writer() = delete;
	UTF8Writer(HANDLE fp, pfWin32Err WinErrFunc);
	~UTF8Writer();

	BOOL append(LPCWSTR text, DWORD cchWideChar);
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
