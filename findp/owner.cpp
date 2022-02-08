#include "stdafx.h"

#include "LastError.h"
#include "beewstring.h"

bee::LastError& GetOwner(LPCWSTR filename, bee::wstring* owner, bee::LastError* lastErr)
{
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
			lastErr->set("GetFileSecurityW(call for size)");
			return *lastErr;
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
		lastErr->set("GetFileSecurityW(second call)", filename);
		LocalFree(pSecDesc);
		return *lastErr;
	}

	ok = IsValidSecurityDescriptor(pSecDesc);
	if (!ok)
	{
		LocalFree(pSecDesc);
		lastErr->set("IsValidSecurityDescriptor", filename);
		return *lastErr;
	}

	PSID psid;
	BOOL lpbOwnerDefaulted = FALSE;
	ok = GetSecurityDescriptorOwner(pSecDesc, &psid, &lpbOwnerDefaulted);
	if (!ok)
	{
		LocalFree(pSecDesc);
		lastErr->set("GetSecurityDescriptorOwner", filename);
		return *lastErr;
	}

	ok = IsValidSid(psid);
	if (!ok)
	{
		LocalFree(pSecDesc);
		lastErr->set("SID is not valid");
		return *lastErr;
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
		lastErr->set("LookupAccountSidW", filename);
		return *lastErr;
	}
	
	owner->assign(ReferencedDomainName).push_back(L'\\').append(Name);

	LocalFree(pSecDesc);

	return *lastErr;
}