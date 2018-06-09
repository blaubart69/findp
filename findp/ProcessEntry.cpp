#include "stdafx.h"
#include "findp.h"
#include "Log.h"

void ProcessEntry(const std::wstring *FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx)
{
	LARGE_INTEGER li;
	li.HighPart = finddata->nFileSizeHigh;
	li.LowPart  = finddata->nFileSizeLow;

	bool matched;
	if ( ctx->opts.matchByRegEx && isFile(finddata->dwFileAttributes) )
	{
		matched = std::regex_search(
			finddata->cFileName
			, *ctx->opts.FilenameRegex.get());

		if (matched)
		{
			InterlockedIncrement64(&ctx->stats.filesMatched);
			InterlockedAdd64(&ctx->stats.sumFileSizeMatched, li.QuadPart);
		}
	}
	else
	{
		matched = true;
	}

	if (!ctx->opts.sum)
	{
		if (  ! ctx->opts.matchByRegEx
			|| (ctx->opts.matchByRegEx && matched && isFile(finddata->dwFileAttributes)))
		{
			PrintEntry(FullBaseDir, finddata);
		}
	}

	if (ctx->opts.SumUpExtensions && isFile(finddata->dwFileAttributes) )
	{
		ProcessExtension(&ctx->ext, finddata->cFileName, li.QuadPart);
	}
}
