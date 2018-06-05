#pragma once

#include "stdafx.h"
#include "ParallelExec.h"

struct Stats
{
	LONGLONG files = 0;
	LONGLONG dirs = 0;
	LONGLONG sumFileSize = 0;
	LONGLONG enumDone = 0;
};

struct Options
{
	std::wstring rootDir;
	bool sum;
	bool progress;
	int maxDepth;
	bool followJunctions;
};

struct Context
{
	Stats	stats;
	Options opts;
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

void ProcessDirectory(DirEntry *item, ParallelExec<DirEntry, Context> *executor, Context *ctx);
void ProcessEntry(const std::wstring *FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx);
int getopts(int argc, wchar_t *argv[], Options* opts);
BOOL TryToSetPrivilege(LPCWSTR szPrivilege, BOOL bEnablePrivilege);