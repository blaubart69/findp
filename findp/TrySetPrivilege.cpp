#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
BOOL TryToSetPrivilege(LPCWSTR szPrivilege, BOOL bEnablePrivilege) {
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
		;
	}
	else if (!LookupPrivilegeValue(NULL
		, szPrivilege
		, &(tp.Privileges[0].Luid))) {
		;
	}
	else if (!AdjustTokenPrivileges(hToken
		, FALSE
		, &tp
		, sizeof(tp)
		, NULL
		, NULL)) {
		;
	}
	else if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		;
	}
	else {
		fSuccess = TRUE;
	} /* endif */

	if (hToken != NULL) CloseHandle(hToken);

	return fSuccess;
}