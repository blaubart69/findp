#define  UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT
#include <windows.h>
#include <stdio.h>

typedef struct _SLIST {
   struct _SLIST *Nxt;
   LONGLONG       Val;
   DWORD          Len;
   WCHAR          Key[ 1 ]; // for the \0
} SLIST;

typedef struct _HT {
    DWORD Entries;
    SLIST *Table[ 1 ];
} HT;


//-------------------------------------------------------------------------------------------------
static DWORD HashValue( LPWSTR s, DWORD *len ) {
//-------------------------------------------------------------------------------------------------

    DWORD hash = 0, l = 0; WCHAR w;

    while (( w = *s++ ) != L'\0' ) {
        hash += w;
        l++;
    } /* endwhile */

    *len = l;

    return hash;
}

//=================================================================================================
BOOL Insert( HT *ht, LPWSTR Key, LONGLONG Val ) {
//=================================================================================================

    DWORD l;
    SLIST *p; 

    DWORD idx = HashValue( Key, &l ) % ht->Entries;

    // search

    p = ht->Table[ idx ];

    while ( p != NULL ) {

        if ( p->Len != l ) {
            ; // no match
        } else if ( memcmp( p->Key, Key, l * sizeof( WCHAR )) != 0 ) {
            ; // no match
        } else {

            // found

            p->Val += Val;
            return FALSE;

        } /* endif */

        p = p->Nxt;

    } /* endwhile */

    // create

    p = HeapAlloc( GetProcessHeap()
                 , 0
                 , sizeof( *p ) + l * sizeof( WCHAR ));

    memcpy( p->Key, Key, ( l + 1 ) * sizeof( WCHAR ));
    p->Val = Val;
    p->Len = l;

    p->Nxt = ht->Table[ idx ];
    ht->Table[ idx ] = p;

    return TRUE;
}

//=================================================================================================
DWORD FreeHT( HT *ht ) {
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
HT* InitHT( DWORD Entries ) {
//=================================================================================================

    HT *ht = HeapAlloc( GetProcessHeap()
                      , HEAP_ZERO_MEMORY
                      , sizeof( *ht ) + ( Entries - 1 ) * sizeof( ht->Table[ 0 ] ));

    ht->Entries = Entries;

    return ht;
}


//=================================================================================================
int wmain( int argc, WCHAR *argv[] ) {
//=================================================================================================

    HT *ht = InitHT( 10 );

    Insert( ht, L"doc", 1 );
    Insert( ht, L"txt", 2 );
    Insert( ht, L"doc", 3 );
    Insert( ht, L"doc", 4 );

    FreeHT( ht );

    return 0;
}

// -=EOF=-
