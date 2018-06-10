#pragma once

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT
#include <windows.h>

typedef struct _SLIST {
	struct _SLIST *Nxt;
	__declspec(align(64)) volatile  LONGLONG       Val;
	DWORD          Len;
	WCHAR          Key[1]; // for the \0
} SLIST;

typedef struct _HT {
	DWORD Entries;
	SLIST *Table[1];
} HT;

typedef struct _HT_STATS {
	DWORD ArrayItems;
	DWORD LongestList;
} HT_STATS;

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*KeyValCallback)(LPWSTR Key, LONGLONG Val, LPVOID context);

HT*		MikeHT_Init		(DWORD Entries);
DWORD	MikeHT_Free		(HT *ht);
BOOL	MikeHT_Insert	(HT *ht, LPWSTR Key, LONGLONG Val);
BOOL    MikeHT_Get		(HT *ht, LPWSTR Key, LONGLONG *Val);
DWORD   MikeHT_ForEach  (HT *ht, KeyValCallback KeyValCallback, HT_STATS *stats, LPVOID context);

#ifdef __cplusplus
}
#endif

