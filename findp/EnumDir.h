#pragma once

template<typename finddataCallback>
DWORD EnumDir(
	LPWSTR fulldir, 
	DWORD fulldirLength, 
	FINDEX_INFO_LEVELS infoLevels, 
	DWORD findex_dwAdditionalFlags, 
	finddataCallback OnDirEntry)
{
	HANDLE hSearch;
	DWORD  dwError = NO_ERROR;
	WIN32_FIND_DATA FindBuffer;

	lstrcpy(fulldir + fulldirLength, L"\\*");
	
	hSearch = FindFirstFileExW(
		fulldir
		, infoLevels
		, &FindBuffer
		, FindExSearchNameMatch
		, NULL
		, findex_dwAdditionalFlags);

	fulldir[fulldirLength] = L'\0';

	if (hSearch == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		if (dwError == ERROR_ACCESS_DENIED)
		{
		}
		else
		{
			Log::Instance()->win32err(L"FindFirstFileExW", fulldir);
		}
		return dwError;
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

	return dwError;
}

