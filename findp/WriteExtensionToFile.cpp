#include "stdafx.h"


void WriteExtensions(const Extensions *ext, UTF8Writer* writer)
{
	HT_STATS stats;
	DWORD itemCount = MikeHT_ForEach(
		ext->extsHashtable,
		[](LPWSTR key, LONGLONG val, LPVOID context)
		{
			UTF8Writer* u8writer = (UTF8Writer*)context;
			u8writer->append(L"%I64d\t%s\r\n", val, key);
			u8writer->write();
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

	UTF8Writer utf8writer(fp, Log::win32errfunc);

	WriteExtensions(ext, &utf8writer);
	CloseHandle(fp);

	Log::Instance()->inf(L"extensions have been written to file %s", filename);
}