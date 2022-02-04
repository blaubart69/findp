#ifndef _UNICODE
#define  UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT
#include <Windows.h>

//-------------------------------------------------------------------------------------------------
LPTSTR* CommandLineToArgv2( LPCTSTR CmdLine, INT *argc ) {
//-------------------------------------------------------------------------------------------------

    //
    // This function is almost identical to the CommandLineToArgv from the shell32.dll
    // Since this is the only function from this DLL we break this dependency and implemented
    // our own function.
    //
    // The special interpretation of backslash characters has NOT be implemented and we use the
    // HeapAlloc() functions, so use HeapFree() instead of LocalFree().
    //

    LPTSTR  *argv;
    LPTSTR  arg_v;
    ULONG   arg_c;
    ULONG   len;
    TCHAR   a;
    ULONG   i;

    BOOLEAN InQuot;
    BOOLEAN InText;

    len = (ULONG)lstrlen( CmdLine );

    // --- we calculate a upper boundary of argc ---

    i = ((( len + 1 ) / 2 ) + 1 ) * sizeof( argv[ 0 ] );
    //                        ^---- for argv[argc]=NULL
    //                  ^---------- for every arg you need min 2 char (argument und deliminator)
    //            ^---------------- round up

    // --- allocate the copy of the commandline and the argv array at once ---

    argv = (LPTSTR*)HeapAlloc( GetProcessHeap(), 0, i + ( len + 2 ) * sizeof( CmdLine[ 0 ] ));

    if ( argv == NULL ) {

        SetLastError( ERROR_NOT_ENOUGH_MEMORY );
        return NULL;

    } /* endif */

    // --- the copy of the commandline tokens starts after the argv array ---

    arg_v = (LPTSTR)( (PUCHAR)argv + i );
    arg_c = 0;

    argv[ 0 ] = arg_v;

    InQuot = FALSE;
    InText = FALSE;

    i = 0;

    while (( a = CmdLine[ i ] ) != TEXT( '\0' )) {

        if ( InQuot ) {

            if ( a == TEXT( '\"' )) {

                // --- special "" handling ---

                if ( CmdLine[ i + 1 ] == TEXT( '\"' )) {
                    *( arg_v++ ) = a; /* Double quote inside quoted string */
                    i++;
                } else {
                    InQuot = FALSE;
                } /* endif */

            } else {
                *( arg_v++ ) = a;
            } /* endif */

        } else {

            switch ( a ) {

                case TEXT( ' '  ):  // "white"
                case TEXT( '\t' ):
                case TEXT( '\n' ):
                case TEXT( '\r' ): {
                    if ( InText ) {
                        *( arg_v++ ) = TEXT( '\0' );
                        InText = FALSE;
                    } /* endif */
                    break;
                }

                case TEXT( '\"' ): {
                    InQuot = TRUE;
                    if ( ! InText ) {
                        argv[ arg_c++ ] = arg_v;
                        InText = TRUE;
                    } /* endif */
                    break;
                }

                default: {
                    if ( ! InText ) {
                        argv[ arg_c++ ] = arg_v;
                        InText = TRUE;
                    } /* endif */
                    *( arg_v++ ) = a;
                    break;
                }

            } /* endswitch */
        } /* endif */

        i++;
    }

    // --- terminate the last token ---

    *arg_v = TEXT( '\0' );

    // --- fill in additional empty argument ---

    argv[ arg_c ] = NULL;

    // --- give back the local results ---

    (*argc) = arg_c;

    return argv;
}

// -=EOF=-
