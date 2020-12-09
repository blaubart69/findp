#pragma once

struct Stats
{
	__declspec(align(8)) volatile LONGLONG files = 0;
	__declspec(align(8)) volatile LONGLONG filesMatched = 0;
	__declspec(align(8)) volatile LONGLONG dirs = 0;
	__declspec(align(8)) volatile LONGLONG sumFileSize = 0;
	__declspec(align(8)) volatile LONGLONG sumFileSizeMatched = 0;
	__declspec(align(8)) volatile LONGLONG enumDone = 0;
	__declspec(align(8)) volatile LONGLONG errAccessDenied = 0;
};

struct Extensions
{
	HT* extsHashtable;

	Extensions() : Extensions(17)
	{
	}

	Extensions(DWORD HastableSize)
	{
		//DWORD HastableSize = 65537;
		extsHashtable = MikeHT_Init(HastableSize);
		Log::Instance()->dbg(L"extension hashtable init with %d array size", HastableSize);
	}

	~Extensions()
	{
		DWORD numberElementsFreed = MikeHT_Free(extsHashtable);
		Log::Instance()->dbg(L"extension hashtable free'd (%d elements)", numberElementsFreed);
	}
};

enum EmitType
{
	Files,
	Dirs,
	Both
};

struct Options
{
	LPWSTR rootDir;
	LPCWSTR FilenameSubstringPattern;
	bool sum;
	bool progress;
	bool followJunctions;
	int maxDepth;
	int ThreadsToUse;
	bool GroupExtensions;
	LPCWSTR ExtsFilename;
	bool printFull;
	bool printOwner;
	EmitType emit;
	LPWSTR extToSearch;
	int extToSearchLen;
	bool quoteFilename;
	FINDEX_INFO_LEVELS findex_info_level;
	DWORD			   findex_dwAdditionalFlags;
};

struct Context
{
	Stats	stats;
	Options opts;
	Extensions* ext;
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


void ProcessDirectory(DirEntryC *item, ParallelExec<DirEntryC, Context,LineWriter> *executor, Context *ctx, LineWriter*);
void ProcessEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx, LineWriter *lineWriter);
void PrintEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, LineWriter *lineWriter, bool printFull, bool printOwner, bool printQuoted);
void ProcessExtension(Extensions *ext, LPCWSTR filename, LONGLONG filesize);
void WriteExtensions(LPCWSTR filename, const Extensions *ext);
DirEntryC* CreateDirEntryC(const DirEntryC *parent, LPCWSTR currentDir);
int getopts(int argc, wchar_t *argv[], Options* opts);
BOOL TryToSetPrivilege(LPCWSTR szPrivilege, BOOL bEnablePrivilege);
bool GetOwner(LPCWSTR filename, LPWSTR owner, size_t ownersize);

