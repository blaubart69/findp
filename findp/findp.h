#pragma once

#include "beevector.h"
#include "beewstring.h"
#include "MikeHash.h"
#include "LastError.h"
#include "ParallelExec.h"

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
		//Log::Instance()->dbg(L"extension hashtable init with %d array size", HastableSize);
	}

	~Extensions()
	{
		DWORD numberElementsFreed = MikeHT_Free(extsHashtable);
		//Log::Instance()->dbg(L"extension hashtable free'd (%d elements)", numberElementsFreed);
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
	LPCWSTR		rootDir;
	std::wstring_view FilenameSubstringPattern;
	bool		sum;
	bool		progress;
	bool		followJunctions;
	int			maxDepth;
	int			ThreadsToUse;
	bool		GroupExtensions;
	LPCWSTR		ExtsFilename;
	bool		printFull;
	bool		printOwner;
	EmitType	emit;
	LPWSTR		extToSearch;
	int			extToSearchLen;
	bool		quoteFilename;
};

struct Context
{
	Stats		 stats;
	Options		 opts;
	Extensions*  ext;
};

/*
typedef struct _LSTR
{
	DWORD	len;
	WCHAR	str[1];
} LSTR;
*/

#ifdef _DEBUG
extern volatile LONGLONG g_HandleOpen;
extern volatile LONGLONG g_HandleClose;
#endif

class SafeHandle
{
public:
	const HANDLE handle;

	SafeHandle(HANDLE h) : handle(h) {};
	~SafeHandle()
	{
		if (handle != nullptr)
		{
#ifdef _DEBUG
			InterlockedIncrement64(&g_HandleClose);
#endif
			CloseHandle(handle);
		}
	}
};

class DirectoryToProcess {
public:
	std::shared_ptr<SafeHandle>		parentHandle;
	std::shared_ptr<bee::wstring>	parentDirectory;
	bee::wstring					directory;
	int								depth;

	DirectoryToProcess(
		  const std::shared_ptr<SafeHandle>& parentHandle
		, const std::shared_ptr<bee::wstring>& parentDirectory
		, LPCWSTR directory
		, size_t  cbDirectoryToEnum
		, int depth)

		: depth(depth)
		, parentHandle(parentHandle)
		, parentDirectory(parentDirectory)
		, directory(directory, cbDirectoryToEnum / sizeof(WCHAR) )
	{
	}
};

class TLS
{
public:
	bee::vector<BYTE>		findBuffer;
	bee::wstring			outBuffer;

	TLS()
	{
		findBuffer.resize(32 * 1024);
	}
};

void ProcessDirectory(DirectoryToProcess *item, ParallelExec<DirectoryToProcess, Context, TLS> *executor, Context *ctx, TLS*);
void ProcessEntry(const bee::wstring& FullBaseDir, nt::FILE_DIRECTORY_INFORMATION* finddata, std::wstring_view filename, Context* ctx, bee::wstring* outBuffer, bee::LastError* lastErr);
void ProcessExtension(Extensions* ext, const std::wstring_view& filename, LONGLONG filesize);
void WriteExtensions(LPCWSTR filename, const Extensions *ext);
int getopts(int argc, wchar_t *argv[], Options* opts);
BOOL TryToSetPrivilege(LPCWSTR szPrivilege, BOOL bEnablePrivilege);
bee::LastError& GetOwner(LPCWSTR filename, bee::wstring* owner, bee::LastError* lastErr);

