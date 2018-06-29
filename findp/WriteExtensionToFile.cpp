#include "stdafx.h"


void WriteExtensions(const Extensions *ext, LineWriter* writer)
{
	HT_STATS stats;
	DWORD itemCount = MikeHT_ForEach(
		ext->extsHashtable,
		[](LPWSTR key, LONGLONG val, LPVOID context)
		{
		    LineWriter* u8writer = (LineWriter*)context;
			u8writer->appendf(L"%I64u\t%s\r\n", val, key);
			u8writer->writeAndReset();
		},
		&stats,
		writer);

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

	LineWriter utf8writer(fp, CP_UTF8, 1024, Log::win32errfunc);

	utf8writer.appendUTF8BOM();
	WriteExtensions(ext, &utf8writer);
	CloseHandle(fp);

	Log::Instance()->inf(L"extensions have been written to file %s", filename);
}