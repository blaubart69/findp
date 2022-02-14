#include "stdafx.h"

#include "Write.h"
#include "findp.h"

void PrintUsage(int);

//WCHAR g_dotDir[2];

int getopts(int argc, wchar_t *argv[], Options* opts)
{
	opts->sum = false;
	opts->progress = false;
	opts->maxDepth = -1;
	opts->followJunctions = false;
	opts->FilenameSubstringPattern;
	opts->ThreadsToUse = 16;
	opts->GroupExtensions = false;
	opts->ExtsFilename = NULL;
	opts->rootDir = NULL;
	opts->printFull = false;
	opts->printOwner = false;
	opts->emit = EmitType::Files;
	opts->extToSearch = NULL;
	opts->extToSearchLen = -1;
	opts->quoteFilename = false;

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
				 case L'q': opts->quoteFilename = true;		break;
				 case L't': if ( i+1 < argc) tmpEmitType = argv[++i];											break;
				 case L'm': if (i + 1 < argc) opts->FilenameSubstringPattern = std::wstring_view(argv[++i]);	break;
				 case L'd': if ( i+1 < argc) opts->maxDepth     = StrToInt((const wchar_t*)argv[++i]);			break;
				 case L'z': if ( i+1 < argc) opts->ThreadsToUse = StrToInt((const wchar_t*)argv[++i]);			break;
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
					 opts->GroupExtensions = true;
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


	 if (tmpEmitType != NULL)
	 {
		 switch ( LOWORD(CharLowerW((LPWSTR)tmpEmitType[0])) )
		 {
		 case L'd': opts->emit = EmitType::Dirs;  break;
		 case L'b': opts->emit = EmitType::Both;  break;
		 case L'f': opts->emit = EmitType::Files; break;
		 }
	 }

	 return 0;
}
void PrintUsage(int threadsToUse)
{
	bee::Out->WriteA(
		"v2.0.0"
		"\nusage: findp.exe [OPTIONS] {directory}"
		"\nOptions:"
		"\n  -f              ... print date, attributes, filesize, fullname"
		"\n  -q              ... enclose filename in double quotes"
		"\n  -o              ... print owner when used with -f"
		"\n  -s              ... sum dirs, files, filesize. don't print filenames"
		"\n  -e [filename]   ... group extensions. 3 columns TAB separated: CountFiles | SumFilesize | Extension (UTF-8)"
		"\n  -p              ... show progress"
		"\n  -j              ... follow directory junctions"
		"\n  -t {f|d|b}      ... emit what  (files|directory|both) default: files"
		"\n  -m {pattern}	  ... substring to match within name. case insensitive. Not in full path."
		"\n  -x {extension}  ... extension to match"
		"\n  -d {depth}      ... how many directories to go down"
		"\n  -z {threads}	  ... threads to start for parallel enumerations. default: 16"
		"\n  -h              ... show this help"
		"\n"
		"\nprepend   \\\\?\\   if you want long path support."
		"\nSamples:"
		"\n  \\\\?\\UNC\\{server}\\{share} for network paths"
		"\n  \\\\?\\c:\\windows"
		"\n");
}