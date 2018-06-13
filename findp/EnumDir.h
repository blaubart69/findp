#pragma once

template<typename finddataCallback>
void EnumDir(LPWSTR fulldir, DWORD fulldirLength, finddataCallback OnDirEntry)
{
	HANDLE hSearch;
	DWORD  dwError = NO_ERROR;
	WIN32_FIND_DATA FindBuffer;

	lstrcpy(fulldir + fulldirLength, L"\\*");
	hSearch = FindFirstFile(fulldir, &FindBuffer);
	fulldir[fulldirLength] = L'\0';

	if (hSearch == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		Log::Instance()->win32err(L"FindFirstFile", fulldir);
		return;
	}

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
		Log::Instance()->win32err(L"FindNextFile", fulldir);
	}

	if (hSearch != INVALID_HANDLE_VALUE)
	{
		FindClose(hSearch);
	}

}

