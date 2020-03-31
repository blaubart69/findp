// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

Log* logger;

void printStats(Stats *stats, bool printMatched);
void printProgress(const ParallelExec<DirEntryC, Context, LineWriter>* executor);
bool CheckIfDirectory(LPCWSTR dirname);
void GetFindExParametersForWindowsVersions(FINDEX_INFO_LEVELS* findex_info_level, DWORD* findex_dwAdditionalFlags);

//int wmain(int argc, wchar_t *argv[])
int beeMain(int argc, wchar_t *argv[])
{
	logger = Log::Instance();
	logger->setLevel(2);
	int rc;

	Context ctx;
	if ((rc = getopts(argc, argv, &ctx.opts)) != 0)
	{
		return rc;
	}

	GetFindExParametersForWindowsVersions(&ctx.opts.findex_info_level, &ctx.opts.findex_dwAdditionalFlags);

	if (!TryToSetPrivilege(SE_BACKUP_NAME, TRUE))
	{
		logger->dbg(L"could not set privilege SE_BACKUP_NAME");
	}

	if (!CheckIfDirectory(ctx.opts.rootDir))
	{
		return 4;
	}

	if (ctx.opts.GroupExtensions)
	{
		ctx.ext = new Extensions(16411);
	}

	int l = lstrlen(ctx.opts.rootDir);

	int lastCharIdx = l - 1;
	if (l > 1 && ctx.opts.rootDir[lastCharIdx] == L'\\')
	{
		ctx.opts.rootDir[lastCharIdx] = L'\0';
	}

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (GetFileType(hStdOut) == FILE_TYPE_DISK)
	{
		if (GetConsoleOutputCP() == CP_UTF8)
		{
			char UTF8BOM[] = { 0xEF, 0xBB, 0xBF };
			DWORD written;
			WriteFile(hStdOut, UTF8BOM, 3, &written, NULL);
		}
	}

	auto queue    = IOCPQueueImpl<DirEntryC>(ctx.opts.ThreadsToUse);
	auto executor = 
		ParallelExec<DirEntryC, Context, LineWriter>(
			&queue
			, []() { return new LineWriter(GetStdHandle(STD_OUTPUT_HANDLE), GetConsoleOutputCP(), 512, Log::win32errfunc); }
			, ProcessDirectory
			, [](LineWriter* usedWriter) { delete usedWriter; }
			, &ctx
			, ctx.opts.ThreadsToUse);

	executor.EnqueueWork( CreateDirEntryC(NULL, ctx.opts.rootDir) );
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

void printProgress(const ParallelExec<DirEntryC, Context, LineWriter>* executor)
{
	long queued, running, done;
	executor->Stats(&queued, &running, &done);

	logger->writeLine(L"queued/running/done %ld/%ld/%ld", queued, running, done);
}

void printStats(Stats *stats, bool printMatched)
{
	WCHAR humanSize[32];

	LPCWSTR seenf    = L"seen    files, filesize, humansize, dirs\t%6I64u %12I64u %s %I64u";
	LPCWSTR matchedf = L"matched files, filesize, humansize      \t%6I64u %12I64u %s";

	logger->writeLine(seenf,
		stats->files,
		stats->sumFileSize,
		StrFormatByteSizeW(stats->sumFileSize, humanSize, 32),
		stats->dirs);

	if (printMatched)
	{
		logger->writeLine(matchedf,
			stats->filesMatched,
			stats->sumFileSizeMatched,
			StrFormatByteSizeW(stats->sumFileSizeMatched, humanSize, 32));
	}

	if (stats->errAccessDenied > 0)
	{
		logger->writeLine(L"FindFirstFileEx access denied:\t%I64u", stats->errAccessDenied);
	}
}

bool CheckIfDirectory(LPCWSTR dirname)
{
	bool rc;

	DWORD rootAttrs = GetFileAttributesW(dirname);
	if (rootAttrs == INVALID_FILE_ATTRIBUTES)
	{
		logger->win32err(L"GetFileAttributes", dirname);
		logger->err(L"directory [%s] does not exist", dirname);

		rc = false;
	}
	else if (!isDirectory(rootAttrs))
	{
		logger->err(L"given parameter [%s] is not a directory", dirname);
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

extern "C" {

	#undef RtlFillMemory
	__declspec(dllimport) VOID __stdcall RtlFillMemory(
		_Out_ VOID UNALIGNED* Destination,
		_In_  SIZE_T         Length,
		_In_  UCHAR          Fill
	);
}

void GetFindExParametersForWindowsVersions(FINDEX_INFO_LEVELS* findex_info_level, DWORD* findex_dwAdditionalFlags)
{
	OSVERSIONINFO osvi;

	//ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	RtlFillMemory(&osvi, sizeof(OSVERSIONINFO), 0);
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	BOOL bIsWindows7orLater =
	   ( (osvi.dwMajorVersion >  6) ||
	   ( (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 1) ));

	logger->dbg(L"Windows version is %d.%d (%d)",
		osvi.dwMajorVersion,
		osvi.dwMinorVersion,
		osvi.dwBuildNumber);

	if ( bIsWindows7orLater )
	{
		*findex_info_level        = FindExInfoBasic;
		*findex_dwAdditionalFlags = FIND_FIRST_EX_LARGE_FETCH;
	}
	else
	{
		*findex_info_level        = FindExInfoStandard;
		*findex_dwAdditionalFlags = 0;
	}
}
