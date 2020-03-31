#include "stdafx.h"

bool EnterDir(DWORD dwFileAttributes, bool FollowJunctions, int currDepth, int maxDepth);
void ConcatDirs(const LSTR *basedir, const LPCWSTR toAppend, LPWSTR out);

void ProcessDirectory(DirEntryC *dirToEnum, ParallelExec<DirEntryC, Context, LineWriter> *executor, Context *ctx, LineWriter *outputLine)
{
	outputLine->reset();
	outputLine->append(dirToEnum->fullDirname.str, dirToEnum->fullDirname.len);

	DWORD errEnumDir = EnumDir(
		  dirToEnum->fullDirname.str
		, dirToEnum->fullDirname.len
		, ctx->opts.findex_info_level
		, ctx->opts.findex_dwAdditionalFlags
		, [dirToEnum, executor, ctx, &outputLine](WIN32_FIND_DATA *finddata)
		{
			if ( isDirectory(finddata->dwFileAttributes) )
			{
				InterlockedIncrement64(&(ctx->stats.dirs));

				if (EnterDir(finddata->dwFileAttributes, ctx->opts.followJunctions, dirToEnum->depth, ctx->opts.maxDepth))
				{
					DirEntryC* newEntry = CreateDirEntryC(dirToEnum, finddata->cFileName);
					executor->EnqueueWork(newEntry);
				}
			}
			else
			{
				InterlockedIncrement64(&(ctx->stats.files));

				LARGE_INTEGER li;
				li.HighPart = finddata->nFileSizeHigh;
				li.LowPart  = finddata->nFileSizeLow;
				InterlockedAdd64(&(ctx->stats.sumFileSize), li.QuadPart);
			}
			ProcessEntry(&dirToEnum->fullDirname, finddata, ctx, outputLine);
		});

	if (errEnumDir == ERROR_ACCESS_DENIED)
	{
		InterlockedIncrement64(&ctx->stats.errAccessDenied);
	}

	HeapFree(GetProcessHeap(), 0, dirToEnum);
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
		if (!FollowJunctions)
		{
			enterDir = false;
		}
	}

	return enterDir;

}

DirEntryC* CreateDirEntryC(const DirEntryC *parent, LPCWSTR currentDir)
{
	DWORD newFullDirLen = 
		  (parent == NULL ? 0 : parent->fullDirname.len + 1 ) // +1 == \ in between 
		+ lstrlen(currentDir);

	DWORD sizeToAlloc =
		  sizeof(DirEntryC)
		+ (		newFullDirLen 
			+	  2		// to append "\*" for searching 
		  ) * sizeof(WCHAR);

	DirEntryC* newEntry;
	if ((newEntry = (DirEntryC*)HeapAlloc(GetProcessHeap(), 0, sizeToAlloc)) == NULL)
	{
		Log::Instance()->win32err(L"HeapAlloc");
	}
	else
	{
		newEntry->depth				= parent == NULL ? 0 : parent->depth + 1;
		newEntry->fullDirname.len	= newFullDirLen;
		ConcatDirs(parent == NULL ? NULL : &parent->fullDirname, currentDir, newEntry->fullDirname.str);
	}
	return newEntry;
}

void ConcatDirs(const LSTR *basedir, const LPCWSTR toAppend, LPWSTR out)
{
	if (basedir != NULL)
	{
		lstrcpy(out, basedir->str);
		out += basedir->len;
		*out = L'\\';
		out++;
	}
	lstrcpy(out, toAppend);
}
