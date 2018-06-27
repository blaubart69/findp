#pragma once

typedef void(*pfWin32Err)(LPCWSTR ApiName, LPCWSTR Param);

BOOL IsDotDir(LPCWSTR cFileName, const DWORD dwFileAttributes);
bool isDirectory(const DWORD dwFileAttributes);
bool isFile(const DWORD dwFileAttributes);
void ReadKey();
int WideCharToUTF8(LPCWSTR text, DWORD cchWideChar, LPSTR lpUTF8Str, int cbUTF8SizeBytes);
void WriteUTF8f(HANDLE fp, LPCWSTR format, ...);