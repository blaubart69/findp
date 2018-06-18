#include "stdafx.h"


void WriteExtensionsToFile(const Extensions *ext, HANDLE fp)
{
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