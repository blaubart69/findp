// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

Log* logger;

void printStats(Stats *stats, bool printMatched);
void printProgress(const ParallelExec<DirEntryC, Context>* executor);
bool CheckIfDirectory(LPCWSTR dirname);


//int wmain(int argc, wchar_t *argv[])
int beeMain(int argc, wchar_t *argv[])
{
	logger = Log::Instance();
	logger->setLevel(2);
	int rc;

	Context ctx;
	if ((rc = getopts(argc, argv, &ctx.opts)) != 0)
	{
		return rc;
	}

	if (!CheckIfDirectory(ctx.opts.rootDir))
	{
		return 4;
	}

	int l = lstrlen(ctx.opts.rootDir);

	int lastCharIdx = l - 1;
	if (l > 1 && ctx.opts.rootDir[lastCharIdx] == L'\\')
	{
		ctx.opts.rootDir[lastCharIdx] = L'\0';
	}


	if (!TryToSetPrivilege(SE_BACKUP_NAME, TRUE))
	{
		logger->wrn(L"could not set privilege SE_BACKUP_NAME");
	}

	auto queue    = std::make_unique< IOCPQueueImpl<DirEntryC> >(ctx.opts.ThreadsToUse);
	auto executor = std::make_unique< ParallelExec<DirEntryC, Context> >(std::move(queue), ProcessDirectory, &ctx, ctx.opts.ThreadsToUse);

	executor->EnqueueWork( CreateDirEntryC(NULL, ctx.opts.rootDir) );
	while (! executor->Wait(1000) )
	{
		if (ctx.opts.progress)
		{
			printProgress(executor.get());
		}
	}

	printStats(&ctx.stats, ctx.opts.FilenameSubstringPattern != NULL);
	if (ctx.opts.SumUpExtensions)
	{
		LPCWSTR extFilename = L".\\exts.txt";
		WriteExtensions(extFilename, &ctx.ext);
	}

    return 0;
}

void printProgress(const ParallelExec<DirEntryC, Context>* executor)
{
	long queued, running, done;
	executor->Stats(&queued, &running, &done);

	logger->writeLine(L"queued/running/done %ld/%ld/%ld", queued, running, done);
}

void printStats(Stats *stats, bool printMatched)
{
	WCHAR humanSize[32];

	logger->resetBuffer();
	logger->appendf(
	   L"dirs/files/filesize"
	   L"\t%I64u/%I64u/%s",
		stats->dirs,
		stats->files,
		StrFormatByteSizeW(stats->sumFileSize, humanSize, 32));

	if (printMatched)
	{
		logger->appendf(
			L" | matched files/filesize"
			L" %I64u/%s",
			stats->filesMatched,
			StrFormatByteSizeW(stats->sumFileSizeMatched, humanSize, 32));
	}

	logger->append(L"\r\n",2);
	logger->writeBuffer();
}

bool CheckIfDirectory(LPCWSTR dirname)
{
	bool rc;

	DWORD rootAttrs = GetFileAttributesW(dirname);
	if (rootAttrs == INVALID_FILE_ATTRIBUTES)
	{
		logger->win32err(L"GetFileAttributes", dirname);
		logger->err(L"directory [%s] does not exist", dirname);

		rc = false;
	}
	else if (!isDirectory(rootAttrs))
	{
		logger->err(L"given parameter [%s] is not a directory", dirname);
		rc = false;
	}
	else
	{
		rc = true;
	}

	return rc;
}
