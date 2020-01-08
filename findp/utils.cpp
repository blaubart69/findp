#include "stdafx.h"

int WideCharToUTF8(LPCWSTR text, DWORD cchWideChar, LPSTR lpUTF8Str, int cbUTF8SizeBytes)
{
	return
		WideCharToMultiByte(
			CP_UTF8
			, 0					// dwFlags [in]
			, text				// lpWideCharStr [in]
			, cchWideChar		// cchWideChar [in]
			, lpUTF8Str			// lpMultiByteStr [out, optional]
			, cbUTF8SizeBytes	// cbMultiByte [in]
			, NULL				// lpDefaultChar[in, optional]
			, NULL);			// lpUsedDefaultChar[out, optional]
}

int convertToMultiByte(const UINT codepage, const WCHAR* text, const DWORD cchWideChar, LPSTR outBuffer, DWORD outBufferSize)
{
	const int bytesWritten = WideCharToMultiByte(
		codepage
		, 0								// dwFlags [in]
		, text							// lpWideCharStr [in]
		, cchWideChar					// cchWideChar [in]
		, outBuffer						// lpMultiByteStr [out, optional]
		, outBufferSize					// cbMultiByte [in]
		, NULL							// lpDefaultChar[in, optional]
		, NULL);

	return bytesWritten;
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

bool isDirectory(const DWORD dwFileAttributes)
{
	return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool isFile(const DWORD dwFileAttributes)
{
	return !isDirectory(dwFileAttributes);
}

bool GetSearchFilterFromDir(LPWSTR str, const size_t strlen, LPWSTR *filter)
{
	return true;
}

void ReadKey()
{
	INPUT_RECORD buffer[64];
	do
	{
		DWORD numberEventsRead = 0;
		ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), buffer, 64, &numberEventsRead);
		//logger->dbg(L"events read: %ld %d %d", numberEventsRead, buffer[0].EventType, buffer[0].Event.KeyEvent.wVirtualKeyCode);

		if (buffer[0].EventType == 1 && buffer[0].Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
		{
			break;
		}
	} while (true);
}

bool endsWith(LPCWSTR string, int cchStringLen, LPCWSTR endToMatch, int cchEndLen)
{
	if (cchEndLen > cchStringLen)
	{
		return false;
	}

	int idxToStartCompare = cchStringLen - cchEndLen;

	int cmp = CompareStringW(
		NULL							//lpLocalName
		, NORM_IGNORECASE				// dwCmpFlags
		, &(string[idxToStartCompare])	// lpString1
		, cchEndLen						// cchCount1
		, endToMatch					// lpString2
		, cchEndLen						// cchCount2
	);

	if (cmp == 0)
	{
		Log::win32errfunc(L"CompareStringW", string);
		return false;
	}

	return cmp == CSTR_EQUAL;
}
bool endsWith(LPCWSTR string, LPCWSTR endToMatch)
{
	return endsWith(
		  string
		, lstrlenW(string)
		, endToMatch
		, lstrlenW(endToMatch));
}