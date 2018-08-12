#pragma once

template<typename finddataCallback>
void EnumDir(LPWSTR dir, DWORD dirLen, LPCWSTR filterPattern, finddataCallback OnDirEntry)
{
	HANDLE hSearch;
	DWORD  dwError = NO_ERROR;
	WIN32_FIND_DATA FindBuffer;

	lstrcpy(dir + dirLen    , L"\\");
	lstrcpy(dir + dirLen + 1, filterPattern);
	hSearch = FindFirstFile(dir, &FindBuffer);

	if (hSearch == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		Log::Instance()->win32err(L"FindFirstFile", dir);
		return;
	}

	dir[dirLen] = L'\0';

	do
	{
		if (!IsDotDir(FindBuffer.cFileName, FindBuffer.dwFileAttributes))
		{
			OnDirEntry(&FindBuffer);
		}

		if (!FindNextFile(hSearch, &FindBuffer))
		{
			dwError = GetLastError();
		}
	} while (dwError != ERROR_NO_MORE_FILES);

	if (dwError != ERROR_NO_MORE_FILES)
	{
		Log::Instance()->win32err(L"FindNextFile", dir);
	}

	if (hSearch != INVALID_HANDLE_VALUE)
	{
		FindClose(hSearch);
	}
}

