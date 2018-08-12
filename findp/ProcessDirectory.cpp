#include "stdafx.h"

bool EnterDir(DWORD dwFileAttributes, bool FollowJunctions, int currDepth, int maxDepth);
void ConcatDirs(const LSTR *basedir, const LPCWSTR toAppend, LPWSTR out);

void ProcessDirectory(DirEntryC *dirToEnum, ParallelExec<DirEntryC, Context, LineWriter> *executor, Context *ctx, LineWriter *outputLine)
{
	outputLine->reset();
	outputLine->append(dirToEnum->fullDirname.str, dirToEnum->fullDirname.len);

	EnumDir(
		dirToEnum->fullDirname.str, 
		dirToEnum->fullDirname.len,
		ctx->opts.filter,
		[dirToEnum, executor, ctx, &outputLine](WIN32_FIND_DATA *finddata)
		{
			if ( isDirectory(finddata->dwFileAttributes) )
			{
				InterlockedIncrement64(&(ctx->stats.dirs));

				if (EnterDir(finddata->dwFileAttributes, ctx->opts.followJunctions, dirToEnum->depth, ctx->opts.maxDepth))
				{
					DirEntryC* newEntry = CreateDirEntryC(dirToEnum, finddata->cFileName, ctx->opts.lenFilter);
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

DirEntryC* CreateDirEntryC(const DirEntryC *parent, LPCWSTR currentDir, const DWORD filterLen)
{
	DWORD newFullDirLen = 
		  (parent == NULL ? 0 : parent->fullDirname.len + 1 ) // +1 == \ in between 
		+ lstrlen(currentDir);

	DWORD sizeToAlloc =
		  sizeof(DirEntryC)
		+ (		newFullDirLen 
			+	1			// to append "\"
			+	filterLen	// to append filter pattern
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
