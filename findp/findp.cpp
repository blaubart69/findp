// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "IConcurrentQueue.h"
#include "IOCPQueueImpl.h"
#include "ParallelExec.h"
#include "Write.h"
#include "findp.h"
#include "utils.h"

void printStats(Stats *stats, bool printMatched);
void printProgress(const ParallelExec<DirectoryToProcess, Context, TLS>* executor);
bool CheckIfDirectory(LPCWSTR dirname);
//void GetFindExParametersForWindowsVersions(FINDEX_INFO_LEVELS* findex_info_level, DWORD* findex_dwAdditionalFlags);

DWORD GetFullName(LPCWSTR filename, bee::wstring* fullname)
{
	DWORD rc = 0;
	DWORD lenNeededPlusZero;
	if ((lenNeededPlusZero = GetFullPathNameW(filename, 0, NULL, NULL)) == 0)
	{
		rc = GetLastError();
	}
	else {
		fullname->resize(lenNeededPlusZero);
		if (GetFullPathNameW(filename, (DWORD)fullname->length(), (LPWSTR)(fullname->data()), NULL) == 0)
		{
			rc = GetLastError();
		}
		fullname->resize(lenNeededPlusZero - 1);
	}
	return rc;
}

//int wmain(int argc, wchar_t *argv[])
int beeMain(int argc, wchar_t *argv[])
{
	int rc;

	Context ctx;
	if ((rc = getopts(argc, argv, &ctx.opts)) != 0)
	{
		return rc;
	}

	if (!TryToSetPrivilege(SE_BACKUP_NAME, TRUE))
	{
		bee::Writer::Err().Write(L"could not set privilege SE_BACKUP_NAME\n");
	}

	bee::wstring FullRootDir;
	GetFullName(ctx.opts.rootDir == NULL ? L"." : ctx.opts.rootDir, &FullRootDir);

	if (!CheckIfDirectory(FullRootDir.c_str()))
	{
		return 4;
	}

	if (ctx.opts.GroupExtensions)
	{
		ctx.ext = new Extensions(16411);
	}

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	auto queue    = IOCPQueueImpl<DirectoryToProcess>(ctx.opts.ThreadsToUse);
	auto executor = 
		ParallelExec<DirectoryToProcess, Context, TLS>(
			&queue
			, []() { return new TLS; }
			, ProcessDirectory
			, [](TLS* tls) { delete tls; }
			, &ctx
			, ctx.opts.ThreadsToUse);

	executor.EnqueueWork( new DirectoryToProcess(nullptr, nullptr, FullRootDir.data(), FullRootDir.length() * sizeof(WCHAR), 0));
	while (! executor.Wait(1000) )
	{
		if (ctx.opts.progress)
		{
			printProgress(&executor);
		}
	}

	bool printMatched = ctx.opts.FilenameSubstringPattern != NULL || ctx.opts.extToSearch != NULL;
	printStats(&ctx.stats, printMatched);
	
	if (ctx.opts.GroupExtensions)
	{
		WriteExtensions(ctx.opts.ExtsFilename, ctx.ext);
		delete ctx.ext;
	}

    return 0;
}

void printProgress(const ParallelExec<DirectoryToProcess, Context, TLS>* executor)
{
	long queued, running, done;
	executor->Stats(&queued, &running, &done);

	//logger->writeLine(L"queued/running/done %ld/%ld/%ld", queued, running, done);
}

void printStats(Stats *stats, bool printMatched)
{
	WCHAR humanSize[32];

	LPCWSTR seenf    = L"seen    files, filesize, humansize, dirs\t%6I64u %12I64u %s %I64u\n";
	LPCWSTR matchedf = L"matched files, filesize, humansize      \t%6I64u %12I64u %s\n";

	bee::wstring tmp;

	tmp.appendf(seenf,
		stats->files,
		stats->sumFileSize,
		StrFormatByteSizeW(stats->sumFileSize, humanSize, 32),
		stats->dirs);

	if (printMatched)
	{
		tmp.appendf(matchedf,
			stats->filesMatched,
			stats->sumFileSizeMatched,
			StrFormatByteSizeW(stats->sumFileSizeMatched, humanSize, 32));
	}
	bee::Writer::Out().Write(tmp);

	if (stats->errAccessDenied > 0)
	{

		tmp.sprintf(L"FindFirstFileEx access denied:\t%I64u\n", stats->errAccessDenied);
		bee::Writer::Err().Write(tmp);
	}
}

bool CheckIfDirectory(LPCWSTR dirname)
{
	bool rc;
	bee::LastError lastErr;

	DWORD rootAttrs = GetFileAttributesW(dirname);
	if (rootAttrs == INVALID_FILE_ATTRIBUTES)
	{
		lastErr.set("GetFileAttributes", dirname);
		lastErr.print();
		rc = false;
	}
	else if (!isDirectory(rootAttrs))
	{
		bee::wstring tmp;
		tmp.sprintf(L"not a directory [%s]\n", dirname);
		bee::Writer::Err().Write(tmp);
		rc = false;
	}
	else
	{
		rc = true;
	}

	return rc;
}
/*
Windows 8.1					6.3*
Windows Server 2012 R2		6.3*
Windows 8					6.2
Windows Server 2012			6.2
Windows 7					6.1
Windows Server 2008 R2		6.1
Windows Server 2008			6.0
Windows Vista				6.0
Windows Server 2003 R2		5.2
Windows Server 2003			5.2
Windows XP 64-Bit Edition	5.2
Windows XP					5.1
Windows 2000				5.0
*/

/*

extern "C" {

	#undef RtlFillMemory
	__declspec(dllimport) VOID __stdcall RtlFillMemory(
		_Out_ VOID UNALIGNED* Destination,
		_In_  SIZE_T         Length,
		_In_  UCHAR          Fill
	);
}
*/