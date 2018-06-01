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

			auto newFullDir = std::make_unique<std::wstring>();
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

	HANDLE quitPressed = CreateEvent(NULL, TRUE, FALSE, NULL);

	Context ctx;
	ctx.sum = opts.sum;

	auto *queue		= new IOCPQueueImpl<DirEntry>();
	auto *executor  = new ParallelExec<DirEntry,Context>(queue, threadEnumFunc, &ctx, quitPressed, 32);

	auto startFullDir = std::make_unique<std::wstring>(opts.rootDir);
	executor->EnqueueWork(new DirEntry(std::move(startFullDir)));

	while (! executor->Wait(1000))
	{
	}

	WCHAR humanSize[32];
	StrFormatByteSizeW(ctx.stats.sumFileSize, humanSize, 32);

	wprintf(L"dirs\t%lld\nfiles\t%lld\nsize\t%lld\t(%s)", 
		ctx.stats.dirs,
		ctx.stats.files, 
		ctx.stats.sumFileSize,
		humanSize);

    return 0;
}

