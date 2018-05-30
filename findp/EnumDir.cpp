#include "stdafx.h"

typedef void(*EnumDirHandler)(WIN32_FIND_DATA *finddata);
BOOL IsDotDir(LPCWSTR cFileName, const DWORD dwFileAttributes);

//-------------------------------------------------------------------------------------------------
void EnumDir(LPCWSTR fulldirnameWithStarAtEnd, const EnumDirHandler OnDirEntry)
//-------------------------------------------------------------------------------------------------
{
	HANDLE hSearch;
	DWORD  dwError = NO_ERROR;
	WIN32_FIND_DATA FindBuffer;

	if ((hSearch = FindFirstFile(fulldirnameWithStarAtEnd, &FindBuffer)) == INVALID_HANDLE_VALUE) {
		dwError = GetLastError();
	}

	while (dwError != ERROR_NO_MORE_FILES) {

		if (dwError != NO_ERROR) {

			//Message(MSGID_ERROR_WINAPI_S, TEXT(__FILE__), __LINE__, L"FindFirst/NextFile", pElement->wcFileName, dwError);
			break;
		}
		else if (!IsDotDir(FindBuffer.cFileName, FindBuffer.dwFileAttributes)) {
			OnDirEntry(&FindBuffer);
		} 

		if (!FindNextFile(hSearch, &FindBuffer)) {
			dwError = GetLastError();
		} 
	} 

	if (hSearch != INVALID_HANDLE_VALUE)
	{
		FindClose(hSearch);
	}
}

//-------------------------------------------------------------------------------------------------
BOOL IsDotDir(LPCWSTR cFileName, const DWORD dwFileAttributes) {
//-------------------------------------------------------------------------------------------------

	if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) return FALSE;
	if (cFileName[0] != L'.')	return FALSE;
	if (cFileName[1] == L'\0')	return TRUE;
	if (cFileName[1] != L'.')	return FALSE;
	if (cFileName[2] == L'\0')	return TRUE;

	return FALSE;
}
