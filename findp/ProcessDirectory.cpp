#include "stdafx.h"

#include "nt.h"
#include "NtEnum.h"
#include "findp.h"
#include "ParallelExec.h"
#include "utils.h"
#include "beewstring.h"
#include "LastError.h"

bool EnterDir(DWORD dwFileAttributes, bool FollowJunctions, int currDepth, int maxDepth);
//void ConcatDirs(const LSTR *basedir, const LPCWSTR toAppend, LPWSTR out);

void ProcessDirectory(DirectoryToProcess *dirToEnum, ParallelExec<DirectoryToProcess, Context, TLS> *executor, Context *ctx, TLS *tls)
{
	nt::NTSTATUS ntstatus;

	HANDLE hDirectory;
	if ( ! NT_SUCCESS(ntstatus = openSubDir(
		&hDirectory
		, dirToEnum->parentHandle == nullptr ? nullptr : dirToEnum->parentHandle->handle
		, dirToEnum->directoryToEnum.data()
		, dirToEnum->directoryToEnum.length() * sizeof(WCHAR))))
	{
		// TODO
	}
	else
	{
		auto currentDirectoryHandle = std::make_shared<SafeHandle>(hDirectory);
		auto currentFullDir         = std::make_shared<bee::wstring>();

		if (dirToEnum->parentDirectory == nullptr)
		{
			currentFullDir->assign(dirToEnum->directoryToEnum);
		}
		else
		{
			currentFullDir->assign( *(dirToEnum->parentDirectory) );
			currentFullDir->push_back(L'\\');
			currentFullDir->append(dirToEnum->directoryToEnum);
		}

		bee::LastError lastErr;

		DWORD errEnumDir = NtEnumDirectory(
			  hDirectory
			, tls->findBuffer.data()
			, tls->findBuffer.size()
			, [&](nt::FILE_DIRECTORY_INFORMATION* finddata)
			{
				if (isDirectory(finddata->FileAttributes))
				{
					InterlockedIncrement64(&(ctx->stats.dirs));

					if (EnterDir(finddata->FileAttributes, ctx->opts.followJunctions, dirToEnum->depth, ctx->opts.maxDepth))
					{
						executor->EnqueueWork(
							new DirectoryToProcess(
								  currentDirectoryHandle
								, currentFullDir
								, &(finddata->FileName[0])
								, (size_t)finddata->FileNameLength
								, dirToEnum->depth + 1
								));
					}
				}
				else // FILE
				{
					InterlockedIncrement64(&(ctx->stats.files));
					InterlockedAdd64(      &(ctx->stats.sumFileSize), finddata->EndOfFile.QuadPart);
				}
				ProcessEntry(*currentFullDir, finddata, ctx, &tls->outBuffer, &lastErr);
			});

		if (errEnumDir == ERROR_ACCESS_DENIED)
		{
			InterlockedIncrement64(&ctx->stats.errAccessDenied);
		}
	}

	delete dirToEnum;
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

/*
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

*/