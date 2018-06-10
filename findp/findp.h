#pragma once

#include "stdafx.h"
#include "ParallelExec.h"
#include "MikeHash.h"
#include "Log.h"
#include "beestr.h"

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
	std::wstring rootDir;
	bool sum;
	bool progress;
	bool followJunctions;
	int maxDepth;
	int ThreadsToUse;
	std::unique_ptr<std::wregex> FilenameRegex;
	bool matchByRegEx;
	bool SumUpExtensions;
};

struct Context
{
	Stats	stats;
	Options opts;
	Extensions ext;
};

struct DirEntry {
public:
	std::unique_ptr<std::wstring> FullDirname;
	const int depth;

	DirEntry(std::unique_ptr<std::wstring>&& dirname, int depthOfDir)
		: FullDirname(std::move(dirname))
		, depth(depthOfDir)
	{
	}
};

typedef struct {
	int		depth;
	BEESTR	fullDirname;
} DirEntryC;


void ProcessDirectory(DirEntry *item, ParallelExec<DirEntry, Context> *executor, Context *ctx);
void ProcessEntry(const std::wstring *FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx);
void PrintEntry(const std::wstring *FullBaseDir, WIN32_FIND_DATA *finddata);
void ProcessExtension(Extensions *ext, LPCWSTR filename, LONGLONG filesize);
void WriteExtensions(LPCWSTR filename, const Extensions *ext);

int getopts(int argc, wchar_t *argv[], Options* opts);
BOOL TryToSetPrivilege(LPCWSTR szPrivilege, BOOL bEnablePrivilege);

inline bool isDirectory(const DWORD dwFileAttributes)
{
	return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}
inline bool isFile(const DWORD dwFileAttributes)
{
	return !isDirectory(dwFileAttributes);
}