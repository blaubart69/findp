#pragma once

BOOL IsDotDir(LPCWSTR cFileName, const DWORD dwFileAttributes);
bool isDirectory(const DWORD dwFileAttributes);
bool isFile(const DWORD dwFileAttributes);
void ReadKey();
int WideCharToUTF8(LPCWSTR text, DWORD len, LPSTR out, int outSizeBytes);
void WriteUTF8f(HANDLE fp, LPCWSTR format, ...);