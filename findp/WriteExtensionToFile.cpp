#include "stdafx.h"


void WriteExtensions(const Extensions *ext, LineWriter* writer)
{
	HT_STATS stats;
	DWORD itemCount = MikeHT_ForEach(
		ext->extsHashtable,
		[](LPWSTR key, LONGLONG Sum, LONGLONG Count, LPVOID context)
		{
		    LineWriter* u8writer = (LineWriter*)context;
			u8writer->writef(L"%I64u\t%I64u\t%s\r\n", Count, Sum, key);
		},
		&stats,
		writer);

	Log::Instance()->dbg(L"Hash: arraysize / items / arrayplaces filled / longest SLIST -- %ld / %ld / %ld / %ld",
		ext->extsHashtable->Entries,
		itemCount,
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

	Log::Instance()->inf(L"Extensions: 3 columns TAB separated (UTF8-BOM): (CountFiles | SumFilesize | Extension). written to file %s.", filename);
}