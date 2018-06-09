// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Log.h"
#include "findp.h"

#include "IConcurrentQueue.h"
#include "IOCPQueueImpl.h"
#include "ParallelExec.h"

#include "EnumDir.h"

Log* logger;

void printStats(Stats *stats, bool printMatched);
void printProgress(const ParallelExec<DirEntry, Context>* executor);
void printExtensions(Extensions *ext);
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

	if (!TryToSetPrivilege(SE_BACKUP_NAME, TRUE))
	{
		logger->wrn(L"could not set privilege SE_BACKUP_NAME");
	}

	auto queue    = std::make_unique< IOCPQueueImpl<DirEntry> >(ctx.opts.ThreadsToUse);
	auto executor = std::make_unique< ParallelExec<DirEntry, Context> >(std::move(queue), ProcessDirectory, &ctx, ctx.opts.ThreadsToUse);

	auto startFullDir = std::make_unique<std::wstring>(ctx.opts.rootDir);
	executor->EnqueueWork(new DirEntry(std::move(startFullDir), 0));

	while (! executor->Wait(1000) )
	{
		if (ctx.opts.progress)
		{
			printProgress(executor.get());
		}
	}

	printStats(&ctx.stats, ctx.opts.matchByRegEx);
	if (ctx.opts.SumUpExtensions)
	{
		printExtensions(&ctx.ext);
	}

    return 0;
}

void printProgress(const ParallelExec<DirEntry, Context>* executor)
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

void printExtensions(Extensions *ext)
{
	logger->writeLine(L"%12I64d\tno extension", ext->noExtSum);

	MikeHT_ForEach(ext->extsHashtable, 
		[](LPWSTR key, LONGLONG val)
		{
			logger->writeLine(L"%12I64d\t%s", val, key);
		});
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

