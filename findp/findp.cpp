// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "IConcurrentQueue.h"
#include "IOCPQueueImpl.h"
#include "ParallelExec.h"
#include "Write.h"
#include "findp.h"
#include "utils.h"

#ifdef _DEBUG
volatile LONGLONG g_HandleOpen;
volatile LONGLONG g_HandleClose;
#endif

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

#ifdef _DEBUG
	g_HandleOpen = 0;
	g_HandleClose = 0;
#endif

	Context ctx;
	if ((rc = getopts(argc, argv, &ctx.opts)) != 0)
	{
		return rc;
	}

	if (!TryToSetPrivilege(SE_BACKUP_NAME, TRUE))
	{
		bee::Err->WriteLine(L"could not set privilege SE_BACKUP_NAME");
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

	bool printMatched = !ctx.opts.FilenameSubstringPattern.empty() || !ctx.opts.extensionToSearch.empty();
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

	//LPCWSTR seenf    = L"seen   \t%6I64u %12s (%I64u) %I64u\r\n";
	//LPCWSTR matchedf = L"matched\t%6I64u %12s (%I64u)\r\n";

	bee::wstring temp;

	temp.append(L"seen   \t")
		.append_ull(stats->files, 12)
		.push_back(L'\t')
		.append(StrFormatByteSizeW(stats->sumFileSize, humanSize, 32))
		.append(L" (")
		.append_ull(stats->sumFileSize)
		.append(L") ")
		.append_ull(stats->dirs)
		.append(L"\r\n");

	if (printMatched)
	{
		temp.append(L"matched\t")
			.append_ull(stats->filesMatched, 12)
			.push_back(L'\t')
			.append(StrFormatByteSizeW(stats->sumFileSizeMatched, humanSize, 32))
			.append(L" (")
			.append_ull(stats->sumFileSizeMatched)
			.append(L")\r\n");
	}
	bee::Err->Write(temp);

	if (stats->errAccessDenied > 0)
	{
		bee::Err->WriteLine( bee::wstring(L"count access denied: ").append_ull(stats->errAccessDenied) );
	}
#ifdef _DEBUG
//	bee::Err->Write(L"handles opened/closed\t%I64u/%I64u\n", g_HandleOpen, g_HandleClose);
#endif

}

bool CheckIfDirectory(LPCWSTR dirname)
{
	bool rc = false;
	bee::LastError lastErr;

	DWORD rootAttrs = GetFileAttributesW(dirname);
	if (rootAttrs == INVALID_FILE_ATTRIBUTES)
	{
		lastErr.set("GetFileAttributes", dirname).print();
	}
	else if (!isDirectory(rootAttrs))
	{
		//bee::Err->Write(L"not a directory [%s]\n", dirname);
		bee::Err->Write(bee::wstring(L"not a directory: ").append(dirname));
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