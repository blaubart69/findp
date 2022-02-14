#include "stdafx.h"

#include "libbee/LastError.h"
#include "libbee/Write.h"
/*
//-------------------------------------------------------------------------------------------------
bee::LastError& TryToSetPrivilege(LPCWSTR szPrivilege, BOOL bEnablePrivilege, bee::LastError *err) {
//-------------------------------------------------------------------------------------------------

	BOOL   fSuccess = FALSE;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid.LowPart = 0;
	tp.Privileges[0].Luid.HighPart = 0;
	tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;

	bee::Err->WriteA("OpenProcessToken\n");
	if (!OpenProcessToken(GetCurrentProcess()
		, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES
		, &hToken)) {
		err->set("OpenProcessToken");
		return *err;
	}

	bee::Err->WriteA("LookupPrivilegeValueW\n");
	if (!LookupPrivilegeValueW(NULL
		, szPrivilege
		, &(tp.Privileges[0].Luid))) {
		err->set("LookupPrivilegeValue");
		return *err;
	}
	bee::Err->WriteA("AdjustTokenPrivileges\n");
	if (!AdjustTokenPrivileges(hToken
		, FALSE
		, &tp
		, sizeof(tp)
		, NULL
		, NULL)) {
		err->set("AdjustTokenPrivileges");
		return *err;
	}
	bee::Err->WriteA("GetLastError\n");
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		bee::Err->WriteA("ERROR_NOT_ALL_ASSIGNED\n");
		err->set("AdjustTokenPrivileges(ERROR_NOT_ALL_ASSIGNED)");
	}
	else {
		fSuccess = TRUE;
	} 

	if (hToken != NULL) CloseHandle(hToken);

	return *err;
}
*/
//-------------------------------------------------------------------------------------------------
bee::LastError& TryToSetPrivilege(LPCWSTR szPrivilege, BOOL bEnablePrivilege, bee::LastError* err) {
//-------------------------------------------------------------------------------------------------

	BOOL   fSuccess = FALSE;
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid.LowPart = 0;
	tp.Privileges[0].Luid.HighPart = 0;
	tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;

	if (!OpenProcessToken(GetCurrentProcess()
		, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES
		, &hToken)) {
		err->set("OpenProcessToken");
	}
	else if (!LookupPrivilegeValue(NULL
		, szPrivilege
		, &(tp.Privileges[0].Luid))) {
		err->set("LookupPrivilegeValue");
	}
	else if (!AdjustTokenPrivileges(hToken
		, FALSE
		, &tp
		, sizeof(tp)
		, NULL
		, NULL)) {
		err->set("AdjustTokenPrivileges");
	}
	else if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		err->set("AdjustTokenPrivileges(ERROR_NOT_ALL_ASSIGNED)");
	}
	else {
		fSuccess = TRUE;
	} 

	if (hToken != NULL) CloseHandle(hToken);

	return *err;
}