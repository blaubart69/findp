#include "stdafx.h"
#include "findp.h"
#include "EnumDir.h"

bool EnterDir(DWORD dwFileAttributes, bool FollowJunctions, int currDepth, int maxDepth);

void ProcessDirectory(DirEntry *item, ParallelExec<DirEntry, Context> *executor, Context *ctx)
{
	EnumDir(item->FullDirname.get(),
		[item, executor, ctx](WIN32_FIND_DATA *finddata)
	{
		if ((finddata->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			InterlockedIncrement64(&(ctx->stats.dirs));

			if (EnterDir(finddata->dwFileAttributes, ctx->opts.followJunctions, item->depth, ctx->opts.maxDepth))
			{
				size_t newLen = item->FullDirname->length() + 1 + lstrlen(finddata->cFileName) + 2;

				auto newFullDir = std::make_unique<std::wstring>();
				newFullDir->reserve(newLen);
				newFullDir->assign(*(item->FullDirname.get()));
				newFullDir->append(L"\\");
				newFullDir->append(finddata->cFileName);

				executor->EnqueueWork(new DirEntry(std::move(newFullDir), item->depth + 1));
			}
		}
		else
		{
			InterlockedIncrement64(&(ctx->stats.files));

			LARGE_INTEGER li;
			li.HighPart = finddata->nFileSizeHigh;
			li.LowPart = finddata->nFileSizeLow;
			InterlockedAdd64(&(ctx->stats.sumFileSize), li.QuadPart);
		}
		if (!ctx->opts.sum)
		{
			ProcessEntry(item->FullDirname.get(), finddata, ctx);
		}
	});
}

bool EnterDir(DWORD dwFileAttributes, bool FollowJunctions, int currDepth, int maxDepth)
{
	bool enterDir = true;

	if (maxDepth > -1)
	{
		if (currDepth + 1 > maxDepth)
		{
			return false;
		}
	}

	if ((dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
	{
		if (FollowJunctions == false)
		{
			enterDir = false;
		}
	}

	return enterDir;

}
