#include "stdafx.h"


void ProcessEntry(LPCWSTR FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx)
{
	LARGE_INTEGER li;
	li.HighPart = finddata->nFileSizeHigh;
	li.LowPart  = finddata->nFileSizeLow;

	bool matched;
	if ( ctx->opts.matchByRegEx && isFile(finddata->dwFileAttributes) )
	{
		std::wregex filenameRegex(ctx->opts.FilenameRegex, std::wregex::flag_type::icase);

		matched = std::regex_search(
			finddata->cFileName
			, filenameRegex);

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
