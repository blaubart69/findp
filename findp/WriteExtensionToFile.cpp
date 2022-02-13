#include "stdafx.h"

#include "Write.h"
#include "beewstring.h"
#include "findp.h"

void WriteExtensionsItems(const Extensions *ext, bee::Writer& writer)
{
	HT_STATS stats;
	DWORD itemCount = MikeHT_ForEach(
		ext->extsHashtable,
		[](LPWSTR key, size_t keyLen, LONGLONG Sum, LONGLONG Count, LPVOID context)
		{
			bee::Writer* writer = (bee::Writer*)context;

			bee::wstring tmp;
			//tmp.sprintf(L"%I64u\t%I64u\t%s\r\n", Count, Sum, key);
			tmp.append_ull(Count)		.push_back('\t')
			   .append_ull(Sum)			.push_back('\t')
			   .append(key, keyLen) 	.append(L"\r\n");
			writer->Write(tmp);
		},
		&stats,
		&writer);
}

void WriteExtensions(LPCWSTR filename, const Extensions *ext)
{
	HANDLE fp;

	if (filename == NULL)
	{
		fp = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	else
	{
		fp = CreateFileW(
			filename
			, GENERIC_WRITE
			, FILE_SHARE_READ
			, NULL
			, CREATE_ALWAYS
			, FILE_ATTRIBUTE_NORMAL
			, NULL);
	}
	if (fp == INVALID_HANDLE_VALUE)
	{
		bee::LastError lastErr;
		lastErr.set("CreateFileW", filename);
		lastErr.print();
		return;
	}

	bee::Writer extWriter(fp, CP_UTF8);

	WriteExtensionsItems(ext, extWriter);
	
	//Log::Instance()->inf(L"Extensions: 3 columns TAB separated (UTF8-BOM): (CountFiles | SumFilesize | Extension). written to file %s.", filename);
}