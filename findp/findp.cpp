// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Log.h"
#include "getopts.h"
#include "common.h"

#include "IConcurrentQueue.h"
#include "IOCPQueueImpl.h"
#include "ParallelExec.h"

#include "EnumDir.h"

Log* logger;

struct DirEntry {
public:
	std::unique_ptr<std::wstring> FullDirname;

	DirEntry(std::unique_ptr<std::wstring>&& dirname) 
		: FullDirname(std::move(dirname))
	{
	}
};

void threadEnumFunc(DirEntry *item, ParallelExec<DirEntry,Context> *executor, Context *ctx)
{
	EnumDir(item->FullDirname.get(), 
		[item, executor, ctx] (WIN32_FIND_DATA *finddata)
	{
		if ( !ctx->sum )
		{
			wprintf(L"%s\\%s\n", item->FullDirname.get()->c_str(), finddata->cFileName);
		}

		if ((finddata->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			InterlockedIncrement64(&(ctx->stats.dirs));

			size_t newLen = item->FullDirname->length() + 1 + lstrlen(finddata->cFileName) + 2;

			auto newFullDir = std::make_unique<std::wstring>();
			newFullDir->reserve(newLen);
			newFullDir->assign(*(item->FullDirname.get()));
			newFullDir->append(L"\\");
			newFullDir->append(finddata->cFileName);

			executor->EnqueueWork( new DirEntry(std::move(newFullDir)) );
		}
		else
		{
			InterlockedIncrement64(&(ctx->stats.files));

			LARGE_INTEGER li;
			li.HighPart = finddata->nFileSizeHigh;
			li.LowPart  = finddata->nFileSizeLow;
			InterlockedAdd64(&(ctx->stats.sumFileSize), li.QuadPart);
		}
	});
}

int wmain(int argc, wchar_t *argv[])
{
	logger = Log::Instance();
	int rc;

	Options opts;
	if ((rc = getopts(argc, argv, &opts)) != 0)
	{
		return rc;
	}

	Context ctx;
	ctx.sum = opts.sum;

	auto queue    = std::make_unique< IOCPQueueImpl<DirEntry> >();
	auto executor = std::make_unique< ParallelExec<DirEntry, Context> >(queue, threadEnumFunc, &ctx, 32);

	auto startFullDir = std::make_unique<std::wstring>(opts.rootDir);
	executor->EnqueueWork(new DirEntry(std::move(startFullDir)));

	while (! executor->Wait(1000) )
	{
	}

	WCHAR humanSize[32];
	StrFormatByteSizeW(ctx.stats.sumFileSize, humanSize, 32);

	wprintf(L"dirs\t%16lld\nfiles\t%16lld\nsize\t%16lld\t(%s)", 
		ctx.stats.dirs,
		ctx.stats.files, 
		ctx.stats.sumFileSize,
		humanSize);

	long startedThreads;
	long endedThreads;
	executor->Stats(&startedThreads, &endedThreads);

	wprintf(L"\nthreads started/ended: %ld/%ld", startedThreads, endedThreads);

    return 0;
}

