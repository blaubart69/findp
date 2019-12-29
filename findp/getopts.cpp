#include "stdafx.h"

void PrintUsage(int);

WCHAR g_dotDir[2];

int getopts(int argc, wchar_t *argv[], Options* opts)
{
	opts->sum = false;
	opts->progress = false;
	opts->maxDepth = -1;
	opts->followJunctions = false;
	opts->FilenameSubstringPattern = NULL;
	opts->ThreadsToUse = 32;
	opts->SumUpExtensions = false;
	opts->ExtsFilename = NULL;
	opts->rootDir = NULL;
	opts->printFull = false;
	opts->printOwner = false;
	opts->emit = EmitType::Files;
	opts->extToSearch = NULL;
	opts->extToSearchLen = -1;

	 bool showHelp = false;
	 LPCWSTR tmpEmitType = NULL;
	 for (int i = 1; i < argc; i++)
	 {
		 if (argv[i][0] == L'-' && argv[i][1] != L'\0')
		 {
			 switch (argv[i][1])
			 {
				default:									break;
				 case L'h': showHelp = true;			    break;
				 case L's': opts->sum = true;				break;
				 case L'p': opts->progress = true;			break;
				 case L'j': opts->followJunctions   = true;	break;
				 case L'f': opts->printFull	 = true;		break;
				 case L'o': opts->printOwner = true;		break;
				 case L'v': Log::Instance()->setLevel(3);	break;
				 case L't': if ( i+1 < argc) tmpEmitType = argv[++i];								   break;
				 case L'm': if ( i+1 < argc) opts->FilenameSubstringPattern = argv[++i];			   break;
				 case L'd': if ( i+1 < argc) opts->maxDepth     = StrToInt((const wchar_t*)argv[++i]); break;
				 case L'z': if ( i+1 < argc) opts->ThreadsToUse = StrToInt((const wchar_t*)argv[++i]); break;
				 case L'x': 
					if (i + 1 < argc)
					{
						LPCWSTR ext = argv[++i];
						int extLen = lstrlen(ext);
						opts->extToSearch = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, extLen + 4); // + dot + zer0
						opts->extToSearch[0] = L'.';
						lstrcpy(&opts->extToSearch[1], ext);
						opts->extToSearchLen = extLen + 1;
					}
					break;
				 case L'e':
					 opts->SumUpExtensions = true;
					 if (i + 1 < argc)
					 {
						 if (argv[i + 1][0] != L'-')
						 {
							 opts->ExtsFilename = argv[++i];
						 }
					 }
					 break;
			 }
		 }
		 else
		 {
			 opts->rootDir = argv[i];
		 }
	 }

	 if (showHelp)
	 {
		 PrintUsage(opts->ThreadsToUse);
		 return 4;
	 }

	 if (opts->rootDir == NULL)
	 {
		 /*
		 Log::Instance()->err(L"no directory given");;
		 PrintUsage();
		 return 2;
		 */
		 lstrcpyW(g_dotDir, L".");
		 opts->rootDir = g_dotDir;
	 }

	 if (opts->FilenameSubstringPattern != NULL)
	 {
		 Log::Instance()->dbg(L"pattern parsed: %s", opts->FilenameSubstringPattern);
	 }
	 if (opts->extToSearch != NULL)
	 {
		 Log::Instance()->dbg(L"extension to search, len: %s, %d", opts->extToSearch, opts->extToSearchLen);
	 }

	 if (tmpEmitType != NULL)
	 {
		 switch ( LOWORD(CharLowerW((LPWSTR)tmpEmitType[0])) )
		 {
		 case L'd': opts->emit = EmitType::Dirs; break;
		 case L'b': opts->emit = EmitType::Both; break;
		 case L'f': opts->emit = EmitType::Files; break;
		 }
	 }

	 return 0;
}
void PrintUsage(int threadsToUse)
{
	Log::Instance()->inf(
		  L"v1.0.5"
		L"\nusage: findp.exe [OPTIONS] {directory}"
		L"\nOptions:"
		L"\n  -f              ... print date, attributes, filesize, fullname"
		L"\n  -o              ... print owner when used with -f"
		L"\n  -s              ... sum dirs, files, filesize. don't print filenames"
		L"\n  -e [filename]   ... group extensions. 3 columns TAB separated: CountFiles | SumFilesize | Extension (UTF-8)"
		L"\n  -p              ... show progress"
		L"\n  -j              ... follow directory junctions"
		L"\n  -v              ... verbose/debug"
		L"\n  -h              ... show this help"
		L"\n  -t {f|d|b}      ... emit what  (files|directory|both) default: files"
		L"\n  -m {pattern}	  ... substring to match within name. case insensitive. Not in full path."
		L"\n  -x {extension}  ... extension to match"
		L"\n  -d {depth}      ... how many directories to go down"
		L"\n  -z {threads}	  ... threads to start for parallel enumerations. default: %d"
		L"\n"
		L"\nprepend   \\\\?\\   if you want to have long path support."
		L"\nSamples:"
		L"\n"
		L"\n          \\\\?\\UNC\\{server}\\{share} for network paths"
		L"\nfindp.exe \\\\?\\c:\\windows"
	, threadsToUse);
}