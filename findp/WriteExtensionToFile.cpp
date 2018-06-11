#include "stdafx.h"

int ConvertToUTF8(LPCWSTR text, DWORD len, LPSTR out, int outSizeBytes)
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

void WriteUTF8f(HANDLE fp, LPCWSTR format, ...)
{
	WCHAR buffer[1024];

	va_list args;
	va_start(args, format);
	int written = wvsprintfW(buffer, format, args);
	va_end(args);

	CHAR utf8buffer[1024];

	int utf8bytes = ConvertToUTF8(buffer, written, utf8buffer, sizeof(utf8buffer));
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

void WriteExtensionsToFile(const Extensions *ext, HANDLE fp)
{
	WriteUTF8f(fp, L"%I64d\tno extension\r\n", ext->noExtSum);

	HT_STATS stats;
	DWORD itemCount = MikeHT_ForEach(
		ext->extsHashtable,
		[](LPWSTR key, LONGLONG val, LPVOID context)
		{
			HANDLE fpToWrite = (HANDLE)context;
			WriteUTF8f(fpToWrite, L"%I64d\t%s\r\n", val, key);
		},
		&stats,
		fp);

	Log::Instance()->dbg(L"Hash: items/arraysize/arrayplaces filled/longest SLIST\t%ld/%ld/%ld/%ld",
		itemCount,
		ext->extsHashtable->Entries,
		stats.ArrayItems,
		stats.LongestList);
}

void WriteExtensions(LPCWSTR filename, const Extensions *ext)
{
	HANDLE fp;

	fp = CreateFileW(
		filename
		, GENERIC_WRITE
		, FILE_SHARE_READ
		, NULL
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	if (fp == INVALID_HANDLE_VALUE)
	{
		Log::Instance()->win32err(L"CreateFileW", filename);
		return;
	}

	WriteExtensionsToFile(ext, fp);
	CloseHandle(fp);

	Log::Instance()->inf(L"extensions have been written to file %s", filename);
}