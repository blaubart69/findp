#pragma once

template<typename finddataCallback>
BOOL EnumDir(LPWSTR fulldir, DWORD fulldirLength, FINDEX_INFO_LEVELS infoLevels, finddataCallback OnDirEntry)
{
	HANDLE hSearch;
	DWORD  dwError = NO_ERROR;
	WIN32_FIND_DATA FindBuffer;

	lstrcpy(fulldir + fulldirLength, L"\\*");
	
	hSearch = FindFirstFileEx(
		fulldir
		, infoLevels
		, &FindBuffer
		, FindExSearchNameMatch
		, NULL
		, FIND_FIRST_EX_LARGE_FETCH);

	fulldir[fulldirLength] = L'\0';

	if (hSearch == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		Log::Instance()->win32err(L"FindFirstFile", fulldir);
		return FALSE;
	}

	BOOL goOnWithEnum = TRUE;
	do
	{
		if (!IsDotDir(FindBuffer.cFileName, FindBuffer.dwFileAttributes))
		{
			goOnWithEnum = OnDirEntry(&FindBuffer);
		}

		if (goOnWithEnum && !FindNextFile(hSearch, &FindBuffer))
		{
			dwError = GetLastError();
		}
	} while (goOnWithEnum && dwError != ERROR_NO_MORE_FILES);

	if (goOnWithEnum && dwError != ERROR_NO_MORE_FILES)
	{
		Log::Instance()->win32err(L"FindNextFile", fulldir);
	}

	if (hSearch != INVALID_HANDLE_VALUE)
	{
		FindClose(hSearch);
	}

	return goOnWithEnum; // FALSE means enumeration has been canceled
}

