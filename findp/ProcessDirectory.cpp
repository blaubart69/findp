#include "stdafx.h"

#include "nt.h"
#include "NtEnum.h"
#include "findp.h"
#include "ParallelExec.h"
#include "utils.h"
#include "beewstring.h"
#include "LastError.h"
#include "Write.h"

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

bee::LastError& OpenDirectoryHandle(DirectoryToProcess* dirToEnum, PHANDLE hDirectory, bee::LastError* err)
{
	if (dirToEnum->parentHandle == nullptr)
	{
		if ((*hDirectory = CreateFileW(
			dirToEnum->directory.c_str()
			, 0
			, 0 // FILE_SHARE_READ
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
	std::shared_ptr<SafeHandle>   currentDirectoryHandle = std::make_shared<SafeHandle>(hDirectory);
	std::shared_ptr<bee::wstring> currentFullDir; 

	//if (!ctx->opts.sum)
	{
		currentFullDir = std::make_shared<bee::wstring>();
		if (dirToEnum->parentDirectory == nullptr)
		{
			currentFullDir->assign(dirToEnum->directory);
			if (currentFullDir->ends_with(L'\\'))
			{
				currentFullDir->resize(currentFullDir->length() - 1);
			}
		}
		else
		{
			currentFullDir->assign(*dirToEnum->parentDirectory);
			currentFullDir->push_back(L'\\');
			currentFullDir->append(dirToEnum->directory);
		}
	}

	bee::LastError lastErr;
	tls->outBuffer.resize(0);

	DWORD rc = NtEnumDirectory(
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
		if (tls->outBuffer.length() > 4096)
		{
			bee::Writer::Out().Write(tls->outBuffer);
			tls->outBuffer.resize(0);
		}
	});

	if (tls->outBuffer.length() > 0)
	{
		bee::Writer::Out().Write(tls->outBuffer);
	}

	return rc;
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
			if (dirToEnum->parentDirectory != nullptr)
			{
				err.param_append(L" (");
				err.param_append(*dirToEnum->parentDirectory);
				err.param_append(L")");
			}
			err.print();
		}
	}
	else
	{
#ifdef _DEBUG
		InterlockedIncrement64(&g_HandleOpen);
#endif
		DWORD rc;
		if ((rc = RunEnumeration(hDirectory, dirToEnum, executor, ctx, tls)) != 0)
		{
			//bee::Writer::Out().Write(L"rc: %d NtQueryDirectoryFile\n", rc);
			err.func("NtQueryDirectoryFile").rc(rc).print();
		}
	}

	delete dirToEnum;
}

