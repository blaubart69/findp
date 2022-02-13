#include "..\MikeHash.h"

#if !defined (InterlockedAdd)

#define InterlockedAdd InterlockedAdd_Inline

LONGLONG
FORCEINLINE
InterlockedAdd_Inline (
    IN OUT LONG volatile *Destination,
    IN LONG Value
    )
{
    LONG Old;

    do {
        Old = *Destination;
    } while ( InterlockedCompareExchange( Destination, Old + Value, Old ) != Old );

    return Old;
}

#endif

//-------------------------------------------------------------------------------------------------
DWORD MikeHT_hash_djb2(const WCHAR *str, DWORD *len) {
//-------------------------------------------------------------------------------------------------

	DWORD hash = 5381;
	
	const WCHAR *p = str;
	while ( *p )
	{
		WCHAR upper = CharUpperW((LPWSTR)*p);
		hash = ( hash * 33 ) ^ upper;	// ((hash << 5) + hash) // hash * 33 ... compiler knows best :-)
		p++;
	}

	*len = p - str;

	return hash;
}

//-------------------------------------------------------------------------------------------------
DWORD MikeHT_hash_djb2_2(const WCHAR* str, size_t str_len) {
//-------------------------------------------------------------------------------------------------

	DWORD hash = 5381;

	const WCHAR* p = str;
	for (; str_len != 0; --str_len)
	{
		WCHAR upper = CharUpperW((LPWSTR)*p);
		hash = (hash * 33) ^ upper;	// ((hash << 5) + hash) // hash * 33 ... compiler knows best :-)
		p++;
	}

	return hash;
}

//-------------------------------------------------------------------------------------------------
DWORD MikeHT_HashValueSimple(const WCHAR *str, DWORD *len) {
//-------------------------------------------------------------------------------------------------

	DWORD hash = 0, l = 0; WCHAR w;

	while ((w = *str++) != L'\0') {
		hash += hash + w;
		l++;
	} /* endwhile */

	*len = l;

	return hash;
}

//-------------------------------------------------------------------------------------------------
static SLIST* FindInList(SLIST *p, LPCWSTR Key, const DWORD cchKeyLen) {
//-------------------------------------------------------------------------------------------------

	while (p != NULL) {

		int cmp;

		if (p->cchKeyLen != cchKeyLen) {
			; // no match
		}
		else if (cchKeyLen == 0)
		{
			break;	// found. two empty strings
		}
		//else if (memcmp(p->Key, Key, KeyLen * sizeof(WCHAR)) != 0) {
		//else if ( lstrcmpiW(p->Key, Key) != 0) {
		else if ((cmp = CompareStringW(NULL, NORM_IGNORECASE, p->Key, p->cchKeyLen, Key, cchKeyLen)) == 0)
		{
			ExitProcess(999);
		}
		else if ( cmp == CSTR_EQUAL)
		{
			break;
		} /* endif */

		p = p->Nxt;

	} /* endwhile */

	return p;
}
//=================================================================================================
BOOL MikeHT_Insert2(HT* ht, LPCWSTR Key, const size_t KeyLen, LONGLONG Val) {
//=================================================================================================

	DWORD idx = MikeHT_hash_djb2_2(Key, KeyLen) % ht->Entries;

	for (;;) {

		SLIST* pNew, * pOld;

		// search

		pOld = pNew = ht->Table[idx];

		pNew = FindInList(pNew, Key, KeyLen);
		if (pNew != NULL)
		{
			// found
			InterlockedAdd64      (&pNew->Sum, Val);
			InterlockedIncrement64(&pNew->Count);
			return FALSE;
		}

		// create

		pNew = (SLIST*)HeapAlloc(GetProcessHeap()
			, 0
			, sizeof(*pNew) + KeyLen * sizeof(WCHAR));

		MoveMemory(pNew->Key, Key, KeyLen * sizeof(WCHAR));
		//pNew->Key[KeyLen] = L'\0';
		pNew->Sum = Val;
		pNew->Count = 1;
		pNew->cchKeyLen = KeyLen;
		pNew->Nxt = pOld;

		// insert

		if (InterlockedCompareExchangePointer((PVOID*)ht->Table + idx
			, pNew
			, pOld) == pOld) {
			return TRUE;
		}

		// retry

		HeapFree(GetProcessHeap(), 0, pNew);

	} /* endfor */
}
//=================================================================================================
DWORD MikeHT_ForEach(HT *ht, KeyValCallback KeyValCallback, HT_STATS *stats, LPVOID context) {
//=================================================================================================

	if (stats != NULL)
	{
		stats->ArrayItems = 0;
		stats->LongestList = 0;
	}

	DWORD ItemCount = 0;

	for (DWORD i = 0; i < ht->Entries; i++)
	{
		if (stats != NULL && ht->Table[i] != NULL)
		{
			stats->ArrayItems++;
		}

		DWORD ListCount = 0;
		for ( SLIST* elem = ht->Table[i]; elem != NULL; elem=elem->Nxt)
		{
			++ItemCount;
			++ListCount;
			KeyValCallback(elem->Key, elem->cchKeyLen, elem->Sum, elem->Count, context);
		}

		if (stats != NULL)
		{
			stats->LongestList = max(stats->LongestList, ListCount);
		}
	}

	return ItemCount;
}
//=================================================================================================
BOOL MikeHT_Get(HT *ht, LPCWSTR Key, LONGLONG *Val) {
//=================================================================================================

	DWORD KeyLen;
	DWORD idx = MikeHT_hash_djb2(Key, &KeyLen) % ht->Entries;

	SLIST *p = ht->Table[idx];

	p = FindInList(p, Key, KeyLen);

	BOOL found;
	if (p == NULL)
	{
		found = FALSE;
	}
	else
	{
		*Val = p->Sum;
		found = TRUE;
	}

	return found;
}
/*
//=================================================================================================
BOOL MikeHT_Insert(HT* ht, LPWSTR Key, LONGLONG Val) {
//=================================================================================================

	DWORD KeyLen;

	//DWORD idx = HashValue( Key, &KeyLen ) % ht->Entries;
	DWORD idx = MikeHT_hash_djb2(Key, &KeyLen) % ht->Entries;

	for (;;) {

		SLIST* pNew, * pOld;

		// search

		pOld = pNew = ht->Table[idx];

		pNew = FindInList(pNew, Key, KeyLen);
		if (pNew != NULL)
		{
			// found
			InterlockedAdd64(&pNew->Sum, Val);
			InterlockedIncrement64(&pNew->Count);
			return FALSE;
		}

		// create

		pNew = (SLIST*)HeapAlloc(GetProcessHeap()
			, 0
			, sizeof(*pNew) + KeyLen * sizeof(WCHAR));

		MoveMemory(pNew->Key, Key, (KeyLen + 1) * sizeof(WCHAR));
		pNew->Sum = Val;
		pNew->Count = 1;
		pNew->cchKeyLen = KeyLen;
		pNew->Nxt = pOld;

		// insert

		if (InterlockedCompareExchangePointer((PVOID*)ht->Table + idx
			, pNew
			, pOld) == pOld) {
			return TRUE;
		}

		// retry

		HeapFree(GetProcessHeap(), 0, pNew);

	} 
}*/


//=================================================================================================
DWORD MikeHT_Free( HT *ht ) {
//=================================================================================================

    DWORD i, n = 0;
    SLIST *p, *pFree;

    for ( i = 0; i < ht->Entries; i++ ) {
        p = ht->Table[ i ];
        while ( p != NULL ) {
            pFree = p;
            p = p->Nxt;
            HeapFree( GetProcessHeap(), 0, pFree );
            n++;
        } /* endwhile */
    } /* endfor */

    HeapFree( GetProcessHeap(), 0, ht );

    return n;
}
//=================================================================================================
HT* MikeHT_Init( DWORD Entries ) {
//=================================================================================================

    HT *ht = (HT*)HeapAlloc( GetProcessHeap()
                      , HEAP_ZERO_MEMORY
                      , sizeof( *ht ) + ( Entries - 1 ) * sizeof( ht->Table[ 0 ] ));

    ht->Entries = Entries;

    return ht;
}

// -=EOF=-
