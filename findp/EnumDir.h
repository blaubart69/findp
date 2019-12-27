#pragma once

template<typename finddataCallback>
void EnumDir(LPWSTR fulldir, DWORD fulldirLength, FINDEX_INFO_LEVELS infoLevels, finddataCallback OnDirEntry)
{
	HANDLE hSearch;
	DWORD  dwError = NO_ERROR;
	WIN32_FIND_DATA FindBuffer;

	lstrcpy(fulldir + fulldirLength, L"\\*");
	
#ifdef _M_AMD64 
	hSearch = FindFirstFileEx(
		fulldir
		, infoLevels
		, &FindBuffer
		, FindExSearchNameMatch
		, NULL
		, FIND_FIRST_EX_LARGE_FETCH);
#else
	hSearch = FindFirstFileEx(
		fulldir
		, infoLevels
		, &FindBuffer
		, FindExSearchNameMatch
		, NULL
		, 0);
#endif // _M_AMD64 


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

