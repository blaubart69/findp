// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

Log* logger;

void printStats(Stats *stats, bool printMatched);
void printProgress(const ParallelExec<DirEntryC, Context>* executor);
bool CheckIfDirectory(LPCWSTR dirname);
void ReadKey();

int wmain(int argc, wchar_t *argv[])
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
	WCHAR x = ctx.opts.rootDir[l - 1];
	WCHAR y = ctx.opts.rootDir[l - 2];
	//if (l > 1 && ctx.opts.rootDir[l - 1] == L'\\')
	if (l > 1) 
	{
		if (L'\\' == L'\\')
		{
			if (x == L'\\')
			{
				ctx.opts.rootDir[l - 1] == L'\0';
			}
		}
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

	logger->write(
	   L"\ndirs/files/filesize"
	   L"\t%I64d/%I64d/%s",
		stats->dirs,
		stats->files,
		StrFormatByteSizeW(stats->sumFileSize, humanSize, 32));

	if (printMatched)
	{
		logger->write(
			L" | matched files/filesize"
			L" %I64d/%s",
			stats->filesMatched,
			StrFormatByteSizeW(stats->sumFileSizeMatched, humanSize, 32));
	}

	logger->write(L"\n");
}

void ReadKey()
{
	logger->dbg(L"press any key");

	INPUT_RECORD buffer[64];
	do
	{
		DWORD numberEventsRead = 0;
		ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), buffer, 64, &numberEventsRead);
		//logger->dbg(L"events read: %ld %d %d", numberEventsRead, buffer[0].EventType, buffer[0].Event.KeyEvent.wVirtualKeyCode);

		if (buffer[0].EventType == 1 && buffer[0].Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
		{
			break;
		}
	} while (true);
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
