#pragma once

typedef void(*pfWin32Err)(LPCWSTR ApiName, LPCWSTR Param);

BOOL IsDotDir(LPCWSTR cFileName, const DWORD dwFileAttributes);
bool isDirectory(const DWORD dwFileAttributes);
bool isFile(const DWORD dwFileAttributes);
void ReadKey();
int convertToMultiByte(const UINT codepage, const WCHAR* text, const DWORD cchWideChar, LPSTR outBuffer, DWORD outBufferSize);
void WriteUTF8f(HANDLE fp, LPCWSTR format, ...);
bool endsWith(LPCWSTR string, int cchStringLen, LPCWSTR endToMatch, int cchEndLen);
bool endsWith(LPCWSTR string, LPCWSTR endToMatch);