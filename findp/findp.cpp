// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

Log* logger;

void printStats(Stats *stats, bool printMatched);
void printProgress(const ParallelExec<DirEntryC, Context, LineWriter>* executor);
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

	if (!TryToSetPrivilege(SE_BACKUP_NAME, TRUE))
	{
		logger->wrn(L"could not set privilege SE_BACKUP_NAME");
	}

	if (!CheckIfDirectory(ctx.opts.rootDir))
	{
		return 4;
	}

	if (ctx.opts.SumUpExtensions)
	{
		ctx.ext = new Extensions(16411);
	}

	int l = lstrlen(ctx.opts.rootDir);

	int lastCharIdx = l - 1;
	if (l > 1 && ctx.opts.rootDir[lastCharIdx] == L'\\')
	{
		ctx.opts.rootDir[lastCharIdx] = L'\0';
	}

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (GetFileType(hStdOut) == FILE_TYPE_DISK)
	{
		if (GetConsoleOutputCP() == CP_UTF8)
		{
			char UTF8BOM[] = { 0xEF, 0xBB, 0xBF };
			DWORD written;
			WriteFile(hStdOut, UTF8BOM, 3, &written, NULL);
		}
	}

	auto queue    = IOCPQueueImpl<DirEntryC>(ctx.opts.ThreadsToUse);
	auto executor = 
		ParallelExec<DirEntryC, Context, LineWriter>(
			&queue
			, []() { return new LineWriter(GetStdHandle(STD_OUTPUT_HANDLE), GetConsoleOutputCP(), 512, Log::win32errfunc); }
			, ProcessDirectory
			, [](LineWriter* usedWriter) { delete usedWriter; }
			, &ctx
			, ctx.opts.ThreadsToUse);

	executor.EnqueueWork( CreateDirEntryC(NULL, ctx.opts.rootDir) );
	while (! executor.Wait(1000) )
	{
		if (ctx.opts.progress)
		{
			printProgress(&executor);
		}
	}

	printStats(&ctx.stats, ctx.opts.FilenameSubstringPattern != NULL);
	if (ctx.opts.SumUpExtensions)
	{
		WriteExtensions(ctx.opts.ExtsFilename, ctx.ext);
		delete ctx.ext;
	}

    return 0;
}

void printProgress(const ParallelExec<DirEntryC, Context, LineWriter>* executor)
{
	long queued, running, done;
	executor->Stats(&queued, &running, &done);

	logger->writeLine(L"queued/running/done %ld/%ld/%ld", queued, running, done);
}

void printStats(Stats *stats, bool printMatched)
{
	WCHAR humanSize[32];

	logger->write(
	   L"dirs, files, filesize, humansize\t%I64u %I64u %I64u %s",
		stats->dirs,
		stats->files,
		stats->sumFileSize,
		StrFormatByteSizeW(stats->sumFileSize, humanSize, 32));

	if (printMatched)
	{
		logger->write(
			L" | matched files, filesize, humansize\t%I64u %I64u %s",
			stats->filesMatched,
			stats->sumFileSizeMatched,
			StrFormatByteSizeW(stats->sumFileSizeMatched, humanSize, 32));
	}

	logger->writeLine(L"");
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
