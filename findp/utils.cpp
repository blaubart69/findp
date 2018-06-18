#include "stdafx.h"

int WideCharToUTF8(LPCWSTR text, DWORD len, LPSTR out, int outSizeBytes)
{
	return
		WideCharToMultiByte(
			CP_UTF8
			, 0				// dwFlags [in]
			, text			// lpWideCharStr [in]
			, len			// cchWideChar [in]
			, out			// lpMultiByteStr [out, optional]
			, outSizeBytes	// cbMultiByte [in]
			, NULL			// lpDefaultChar[in, optional]
			, NULL);		// lpUsedDefaultChar[out, optional]
}

int CalcUTF8sizeBytes(LPCWSTR text, DWORD len)
{
	return WideCharToUTF8(text, len, NULL, 0);
}

void WriteUTF8f(HANDLE fp, LPCWSTR format, ...)
{
	WCHAR buffer[1024];

	va_list args;
	va_start(args, format);
	int written = wvsprintfW(buffer, format, args);
	va_end(args);

	CHAR utf8buffer[1024];

	int utf8bytes = WideCharToUTF8(buffer, written, utf8buffer, sizeof(utf8buffer));
	if (utf8bytes == 0)
	{
		Log::Instance()->win32err(L"WideCharToMultiByte", buffer);
	}
	else
	{
		DWORD writtenToFile;
		WriteFile(fp, utf8buffer, utf8bytes, &writtenToFile, NULL);
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

bool isDirectory(const DWORD dwFileAttributes)
{
	return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool isFile(const DWORD dwFileAttributes)
{
	return !isDirectory(dwFileAttributes);
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