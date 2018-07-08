#include "stdafx.h"


void ProcessEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx, LineWriter *lineWriter)
{
	ULARGE_INTEGER li;
	li.HighPart = finddata->nFileSizeHigh;
	li.LowPart  = finddata->nFileSizeLow;

	bool matched;
	if ( ctx->opts.FilenameSubstringPattern != NULL && isFile(finddata->dwFileAttributes) )
	{
		matched = StrStrIW(finddata->cFileName, ctx->opts.FilenameSubstringPattern) != NULL;
		if (matched)
		{
			matched = true;
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
		if (    ctx->opts.FilenameSubstringPattern == NULL
			|| (ctx->opts.FilenameSubstringPattern != NULL && matched && isFile(finddata->dwFileAttributes)))
		{
			if (   (ctx->opts.emit == EmitType::Both)
				|| (ctx->opts.emit == EmitType::Files && isFile     (finddata->dwFileAttributes))
				|| (ctx->opts.emit == EmitType::Dirs  && isDirectory(finddata->dwFileAttributes))  )
			{
				PrintEntry(FullBaseDir, finddata, lineWriter, ctx->opts.printFull);
			}
		}
	}

	if (ctx->opts.SumUpExtensions && isFile(finddata->dwFileAttributes) )
	{
		ProcessExtension(&ctx->ext, finddata->cFileName, li.QuadPart);
	}
}
