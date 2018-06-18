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
		WCHAR upper = LOWORD(CharUpperW((LPWSTR)*p));
		hash = ( hash * 33 ) ^ upper;	// ((hash << 5) + hash) // hash * 33 ... compiler knows best :-)
		p++;
	}

	*len = p - str;

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
static SLIST* FindInList(SLIST *p, LPCWSTR Key, const DWORD KeyLen) {
//-------------------------------------------------------------------------------------------------

	while (p != NULL) {

		if (p->Len != KeyLen) {
			; // no match
		}
		//else if (memcmp(p->Key, Key, KeyLen * sizeof(WCHAR)) != 0) {
		else if ( lstrcmpiW(p->Key, Key) != 0) {
			; // no match
		}
		else {

			// found

			break;

		} /* endif */

		p = p->Nxt;

	} /* endwhile */

	return p;
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
			ItemCount++;
			ListCount++;
			KeyValCallback(elem->Key, elem->Val, context);
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
		*Val = p->Val;
		found = TRUE;
	}

	return found;
}

//=================================================================================================
BOOL MikeHT_Insert( HT *ht, LPWSTR Key, LONGLONG Val ) {
//=================================================================================================

    DWORD KeyLen;

    //DWORD idx = HashValue( Key, &KeyLen ) % ht->Entries;
	DWORD idx = MikeHT_hash_djb2(Key, &KeyLen) % ht->Entries;

    for (;;) {

		SLIST *pNew, *pOld;

        // search

        pOld = pNew = ht->Table[ idx ];

		pNew = FindInList(pNew, Key, KeyLen);
		if (pNew != NULL)
		{
			// found
			InterlockedAdd64(&pNew->Val, Val);
			return FALSE;
		}

        // create

        pNew = (SLIST*)HeapAlloc( GetProcessHeap()
                     , 0
                     , sizeof( *pNew ) + KeyLen * sizeof( WCHAR ));

        memcpy( pNew->Key, Key, ( KeyLen + 1 ) * sizeof( WCHAR ));
        pNew->Val = Val;
        pNew->Len = KeyLen;
        pNew->Nxt = pOld;

        // insert

        if ( InterlockedCompareExchangePointer( (PVOID*)ht->Table + idx
                                              , pNew
                                              , pOld ) == pOld ) {
            return TRUE;
        } /* endif */

        // retry

        HeapFree( GetProcessHeap(), 0, pNew );

    } /* endfor */
}

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
