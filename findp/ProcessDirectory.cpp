#include "stdafx.h"
#include "findp.h"
#include "EnumDir.h"
#include "Log.h"

bool EnterDir(DWORD dwFileAttributes, bool FollowJunctions, int currDepth, int maxDepth);

void ProcessDirectory(DirEntry *item, ParallelExec<DirEntry, Context> *executor, Context *ctx)
{
	EnumDir(item->FullDirname.get(),
		[item, executor, ctx](WIN32_FIND_DATA *finddata)
	{
		if ( isDirectory(finddata->dwFileAttributes) )
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
		ProcessEntry(item->FullDirname.get(), finddata, ctx);
	});

	delete item;
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

DirEntry_C* CreateDirEntryC(const DirEntry_C *parent, LPCWSTR currentDir, int currDepth)
{
	int newEntryDirLen = 
		(parent == NULL ? 
			0								
			: parent->fullDirnameLen 
				+ 1	// + "\"
		)
		+ lstrlen(currentDir);

	int sizeToAlloc =
		sizeof(DirEntry_C)
		+ 	(
			newEntryDirLen
			+ 2	// "\*"
			) 
			* sizeof(WCHAR)
		;

	DirEntry_C* newEntry;
	if ((newEntry = (DirEntry_C*)HeapAlloc(GetProcessHeap(), 0, sizeToAlloc)) == NULL)
	{
		Log::Instance()->win32err(L"HeapAlloc");
	}
	else
	{
		newEntry->depth = currDepth;
		newEntry->fullDirnameLen = newEntryDirLen;

		WCHAR *writer = newEntry->fullDirname;
		if (parent != NULL)
		{
			lstrcpy(writer, parent->fullDirname);
			writer += parent->fullDirnameLen;
			*writer = L'\\';
			writer++;
		}
		lstrcpy(writer, currentDir);
	}
	return newEntry;
}
