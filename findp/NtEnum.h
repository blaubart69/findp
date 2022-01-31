#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "nt.h"

//#include "beevector.h"
//#include "beewstring.h"

//typedef void (*EntryCallback)(const std::wstring& dir, size_t rootDirLen, FILE_DIRECTORY_INFORMATION* info);
//typedef std::function<void(bee::wstring& dir, size_t rootDirLen, FILE_DIRECTORY_INFORMATION* info)> EntryCallback_t;

#define FIND_BUF_LENGTH (16*1024)

/*
template<typename C>
class NtEnum
{
public:
    NtEnum(const C onEntry) : _pfnOnEntry(onEntry) {}
	DWORD Run(bee::wstring& dir);

private:
	bee::wstring		    _dir;
    size_t                  _rootDirLen;
	const C	                _pfnOnEntry;
	bee::vector< BYTE* >	_findBufs;

	DWORD _internal_enum_recurse(const HANDLE hDir, const size_t depth);
};
*/

template<class T, class C>
void walk_NextEntryOffset_buffer(PVOID buf, size_t length, C onEntry)
{
	T* info = (T*)buf;

	for (;;)
	{
		onEntry(info);

		if (info->NextEntryOffset == 0)
		{
			break;
		}
		else
		{
			info = (T*)((BYTE*)info + info->NextEntryOffset);
		}
	}
}

bool isDotOrDotDot(const ULONG fileattributes, LPCWSTR filename, const ULONG cbLen)
{
	if ((fileattributes & FILE_ATTRIBUTE_DIRECTORY) == 0) return false;

	if (filename[0] != L'.')		return false;
	if (cbLen == 1 * sizeof(WCHAR))	return true;
	if (filename[1] != L'.')		return false;
	if (cbLen == 2 * sizeof(WCHAR))	return true;

	return false;
}

template<class Fn>
void walkDirectoryInformation(PVOID buf, size_t length, Fn onFindData)
{
	nt::FILE_DIRECTORY_INFORMATION* info = (nt::FILE_DIRECTORY_INFORMATION*)buf;

	for (;;)
	{
		if (isDotOrDotDot(info->FileAttributes, info->FileName, info->FileNameLength))
		{
		}
		else
		{
			onFindData(info);
		}

		if (info->NextEntryOffset == 0)
		{
			break;
		}
		else
		{
			info = (nt::FILE_DIRECTORY_INFORMATION*)((char*)info + info->NextEntryOffset);
		}
	}
}

/*
template<typename C>
DWORD NtEnum<C>::Run(bee::wstring& dir)
{
	DWORD rc = 0;

	HANDLE hDir;
	if ((hDir = CreateFileW(
		dir.c_str()
		, GENERIC_READ
		, FILE_SHARE_READ
		, NULL
		, OPEN_EXISTING
		, FILE_FLAG_BACKUP_SEMANTICS
		, NULL)) == INVALID_HANDLE_VALUE)
	{
		rc = GetLastError();
	}
	else
	{
		if (dir.ends_with(L'\\'))
		{
			_dir.assign(dir.data(), dir.length() - 1);
		}
		else
		{
			_dir.assign(dir);
		}
		_rootDirLen = dir.length();
		rc = _internal_enum_recurse(hDir, 0);
	}
	return rc;
}
*/

//#include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\km\wdm.h"
//#include "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\km\ntifs.h"

nt::NTSTATUS openSubDir(PHANDLE hSubDir, HANDLE hParentDir, PWSTR filename, ULONG cbFilenameLen)
{
	nt::UNICODE_STRING	uStrFilename;
	uStrFilename.Buffer				= filename;
	uStrFilename.Length				= (USHORT)cbFilenameLen;
	uStrFilename.MaximumLength		= (USHORT)cbFilenameLen;

	nt::OBJECT_ATTRIBUTES oAttr;
	oAttr.Length					= sizeof(nt::OBJECT_ATTRIBUTES);
	oAttr.RootDirectory				= hParentDir;
	oAttr.Attributes				= 0;
	oAttr.ObjectName				= &uStrFilename;
	oAttr.SecurityDescriptor		= NULL;
	oAttr.SecurityQualityOfService	= NULL;

	nt::IO_STATUS_BLOCK io_status_block;

	return NtOpenFile(
		hSubDir
		, GENERIC_READ // ACCESS_MASK
		, &oAttr
		, &io_status_block
		, FILE_SHARE_READ
		, FILE_DIRECTORY_FILE | FILE_OPEN_FOR_BACKUP_INTENT);
}


#define STATUS_SUCCESS                   ((nt::NTSTATUS)0x00000000L)    // ntsubauth
#define STATUS_NO_MORE_FILES             ((nt::NTSTATUS)0x80000006L)

//typedef void (*cbFileDirectoryInformation)(nt::FILE_DIRECTORY_INFORMATION* info);

template<typename C>
DWORD NtEnumDirectory(
	  const HANDLE hDir
	, PVOID buf
	, const ULONG bufLength
	, C pfnFileDirectoryInformation
) {
	nt::NTSTATUS status;
	nt::IO_STATUS_BLOCK io_status_block;
	io_status_block.Information = 0;
	io_status_block.Pointer = 0;
	io_status_block.Status = 0;

	DWORD rc = 0;
	for (;;)
	{
		if ((status = nt::NtQueryDirectoryFile(
			hDir
			, NULL	  // event
			, NULL	  // APC routine
			, NULL	  // APC context
			, &io_status_block
			, buf
			, bufLength
			, nt::FileDirectoryInformation
			, FALSE	  // ReturnSingleEntry
			, NULL    // Filename
			, FALSE)) // RestartScan
			== ((nt::NTSTATUS)0x80000006L) )		// #define STATUS_NO_MORE_FILES             ((NTSTATUS)0x80000006L)
		{
			rc = ERROR_SUCCESS;
			break;
		}
		else if (status != ((nt::NTSTATUS)0x00000000L))
		{
			rc = nt::RtlNtStatusToDosError(status);
			// todo: error
			break;
		}
		else if (io_status_block.Information == 0)
		{
			rc = ERROR_INSUFFICIENT_BUFFER;
			break;
		}
		else
		{
			/*
			walkDirectoryInformation(
				buf
				, io_status_block.Information
				, [&](nt::FILE_DIRECTORY_INFORMATION* info)
			{
					pfnFileDirectoryInformation(info);
			}); */

			walk_NextEntryOffset_buffer<nt::FILE_DIRECTORY_INFORMATION>(
				  buf
				, io_status_block.Information, 
				[&](nt::FILE_DIRECTORY_INFORMATION* info)
				{
					pfnFileDirectoryInformation(info);
				});
		}
	}

	return rc;
}
