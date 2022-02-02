#include "stdafx.h"

#include "nt.h"
#include "NtEnum.h"
#include "findp.h"
#include "ParallelExec.h"
#include "utils.h"
#include "beewstring.h"
#include "LastError.h"

bool EnterDir(DWORD dwFileAttributes, bool FollowJunctions, int currDepth, int maxDepth);

bee::LastError& OpenDirectoryHandle(DirectoryToProcess* dirToEnum, PHANDLE hDirectory, bee::LastError* err)
{
	if (dirToEnum->parentHandle == nullptr)
	{
		if ((*hDirectory = CreateFileW(
			dirToEnum->directory.c_str()
			, GENERIC_READ
			, FILE_SHARE_READ
			, NULL
			, OPEN_EXISTING
			, FILE_FLAG_BACKUP_SEMANTICS
			, NULL)) == INVALID_HANDLE_VALUE)
		{
			err->func("CreateFileW").param(dirToEnum->directory);
		}
	}
	else
	{
		nt::NTSTATUS ntstatus;
		if ( ! NT_SUCCESS(ntstatus = openSubDir(
			  hDirectory
			, dirToEnum->parentHandle->handle
			, dirToEnum->directory.data()
			, dirToEnum->directory.length() * sizeof(WCHAR))))
		{
			err->func("NtOpenFile").rc_from_NTSTATUS(ntstatus).param(dirToEnum->directory);
		}
	}

	return *err;
}

DWORD RunEnumeration(HANDLE hDirectory, DirectoryToProcess* dirToEnum, ParallelExec<DirectoryToProcess, Context, TLS>* executor, Context* ctx, TLS* tls)
{
	auto currentDirectoryHandle = std::make_shared<SafeHandle>(hDirectory);
	auto currentFullDir         = std::make_shared<bee::wstring>();

	if (dirToEnum->parentDirectory == nullptr)
	{
		currentFullDir->assign(dirToEnum->directory);
	}
	else
	{
		currentFullDir->assign(*dirToEnum->parentDirectory);
		currentFullDir->push_back(L'\\');
		currentFullDir->append(dirToEnum->directory);
	}

	bee::LastError lastErr;

	return NtEnumDirectory(
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
			InterlockedAdd64(&(ctx->stats.sumFileSize), finddata->EndOfFile.QuadPart);
		}
		ProcessEntry(*currentFullDir, finddata, ctx, &tls->outBuffer, &lastErr);
	});
}

void ProcessDirectory(DirectoryToProcess *dirToEnum, ParallelExec<DirectoryToProcess, Context, TLS> *executor, Context *ctx, TLS *tls)
{
	bee::LastError err;

	HANDLE hDirectory = nullptr;
	if ( OpenDirectoryHandle(dirToEnum, &hDirectory, &err).failed() )
	{
		if (err.code() == ERROR_ACCESS_DENIED)
		{
			InterlockedIncrement64(&ctx->stats.errAccessDenied);
		}
		else
		{
			err.print();
		}
	}
	else
	{
		RunEnumeration(hDirectory, dirToEnum, executor, ctx, tls);
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