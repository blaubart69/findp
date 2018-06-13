#pragma once

struct Stats
{
	__declspec(align(64)) volatile LONGLONG files = 0;
	__declspec(align(64)) volatile LONGLONG filesMatched = 0;
	__declspec(align(64)) volatile LONGLONG dirs = 0;
	__declspec(align(64)) volatile LONGLONG sumFileSize = 0;
	__declspec(align(64)) volatile LONGLONG sumFileSizeMatched = 0;
	__declspec(align(64)) volatile LONGLONG enumDone = 0;
};

struct Extensions
{
	__declspec(align(64)) volatile LONGLONG noExtSum = 0;
	//std::unordered_map<std::wstring, LONGLONG> exts;
	HT* extsHashtable;

	Extensions()
	{
		DWORD HastableSize = 65537;
		extsHashtable = MikeHT_Init(HastableSize);
		Log::Instance()->dbg(L"extension hashtable init with %d array size", HastableSize);
	}

	~Extensions()
	{
		DWORD numberElementsFreed = MikeHT_Free(extsHashtable);
		Log::Instance()->dbg(L"extension hashtable free'd (%d elements)", numberElementsFreed);
	}
};

struct Options
{
	LPCWSTR rootDir;
	LPCWSTR FilenameSubstringPattern;
	bool sum;
	bool progress;
	bool followJunctions;
	int maxDepth;
	int ThreadsToUse;
	bool SumUpExtensions;
};

struct Context
{
	Stats	stats;
	Options opts;
	Extensions ext;
};

typedef struct _LSTR
{
	DWORD	len;
	WCHAR	str[1];
} LSTR;

typedef struct _DirEntryC {
	int		depth;
	LSTR	fullDirname;
} DirEntryC;


void ProcessDirectory(DirEntryC *item, ParallelExec<DirEntryC, Context> *executor, Context *ctx);
void ProcessEntry(LPCWSTR FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx);
void PrintEntry(LPCWSTR FullBaseDir, WIN32_FIND_DATA *finddata);
void ProcessExtension(Extensions *ext, LPCWSTR filename, LONGLONG filesize);
void WriteExtensions(LPCWSTR filename, const Extensions *ext);
DirEntryC* CreateDirEntryC(const DirEntryC *parent, LPCWSTR currentDir);
int getopts(int argc, wchar_t *argv[], Options* opts);
BOOL TryToSetPrivilege(LPCWSTR szPrivilege, BOOL bEnablePrivilege);

