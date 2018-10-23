#include "stdafx.h"

bool GetOwner(LPCWSTR filename, LPWSTR owner, size_t ownersize)
{
	Log* log = Log::Instance();

	DWORD lenNeeded;
	BOOL ok;

	ok = GetFileSecurityW(
		filename
		, OWNER_SECURITY_INFORMATION	// RequestedInformation
		, NULL							// pSecurityDescriptor
		, 0								// nLength,
		, &lenNeeded					// lpnLengthNeeded
	);
	if (!ok)
	{
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER )
		{
			log->win32errfunc(L"GetFileSecurityW(call for size)", filename);
			return false;
		}
	}

	PSECURITY_DESCRIPTOR pSecDesc;
	pSecDesc = LocalAlloc(LMEM_FIXED, lenNeeded);

	DWORD lenNeeded2;
	ok = GetFileSecurityW(
		filename
		, OWNER_SECURITY_INFORMATION	// RequestedInformation
		, pSecDesc						// pSecurityDescriptor
		, lenNeeded						// nLength,
		, &lenNeeded2					// lpnLengthNeeded
	);
	if (!ok)
	{
		log->win32errfunc(L"GetFileSecurityW(second call)", filename);
		LocalFree(pSecDesc);
		return false;
	}

	ok = IsValidSecurityDescriptor(pSecDesc);
	if (!ok)
	{
		LocalFree(pSecDesc);
		log->win32errfunc(L"IsValidSecurityDescriptor", filename);
		return false;
	}

	PSID psid;
	BOOL lpbOwnerDefaulted = FALSE;
	ok = GetSecurityDescriptorOwner(pSecDesc, &psid, &lpbOwnerDefaulted);
	if (!ok)
	{
		LocalFree(pSecDesc);
		log->win32errfunc(L"GetSecurityDescriptorOwner", filename);
		return false;
	}

	ok = IsValidSid(psid);
	if (!ok)
	{
		LocalFree(pSecDesc);
		log->err(L"SID is not valid");
		return false;
	}

	WCHAR Name[128];
	DWORD cchName = sizeof(Name) / 2;

	WCHAR ReferencedDomainName[128];
	DWORD cchReferencedDomainName = sizeof(ReferencedDomainName) / 2;

	SID_NAME_USE sid_name_use;

	ok = LookupAccountSidW(
		NULL		// lpSystemName
		, psid
		, Name
		, &cchName
		, ReferencedDomainName
		, &cchReferencedDomainName
		, &sid_name_use
	);
	if (!ok)
	{
		LocalFree(pSecDesc);
		log->win32errfunc(L"LookupAccountSidW", filename);
		return false;
	}
	
	wsprintfW(owner, L"%s\\%s", ReferencedDomainName, Name);

	LocalFree(pSecDesc);

	return ok;
}