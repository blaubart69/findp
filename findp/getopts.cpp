#include "stdafx.h"

void PrintUsage(void);

int getopts(int argc, wchar_t *argv[], Options* opts)
{
	 if (argc < 2)
	 {
		 return 1;
	 }

	 opts->sum = false;
	 opts->progress = false;
	 opts->maxDepth = -1;
	 opts->followJunctions = false;
	 opts->FilenameSubstringPattern = NULL;
	 opts->ThreadsToUse = 32;
	 opts->SumUpExtensions = false;
	 opts->rootDir = NULL;

	 bool showHelp = false;

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
				 case L'j': opts->followJunctions = true;	break;
				 case L'e': opts->SumUpExtensions = true;	break;
				 case L'v': Log::Instance()->setLevel(3);	break;
				 case L'm': if ( i+1 < argc) opts->FilenameSubstringPattern = argv[++i];			   break;
				 case L'd': if ( i+1 < argc) opts->maxDepth     = StrToInt((const wchar_t*)argv[++i]); break;
				 case L't': if ( i+1 < argc) opts->ThreadsToUse = StrToInt((const wchar_t*)argv[++i]); break;
			 }
		 }
		 else
		 {
			 opts->rootDir = argv[i];
		 }
	 }

	 if (showHelp)
	 {
		 PrintUsage();
		 return 4;
	 }

	 if (opts->rootDir == NULL)
	 {
		 Log::Instance()->err(L"no directory given");;
		 PrintUsage();
		 return 2;
	 }

	 if (opts->FilenameSubstringPattern != NULL)
	 {
		 Log::Instance()->dbg(L"pattern parsed: %s", opts->FilenameSubstringPattern);
	 }

	 return 0;
}
void PrintUsage(void)
{
	Log::Instance()->inf(
		L"usage: findp.exe [OPTIONS] {directory}"
		L"\n-s ... sum dirs, files, filesize. don't print filenames"
		L"\n-e ... group extensions and calc the sum of the filesizes. write to .\\exts.txt (UTF-8)"
		L"\n-p ... show progress"
		L"\n-j ... follow directory junctions"
		L"\n-v ... verbose/debug"
		L"\n-h ... show this help"
		L"\n-m {pattern}		... substring to match within filename. case insensitive"
		L"\n-d {depth}          ... how many directory to go down"
		L"\n-t {numberThreads}	... threads to start for parallel enumerations"
	    L"\n\nSamples:"
		L"\n"
		L"\nprepend   \\\\?\\ if you want to have long path support."
		L"\nOr        \\\\?\\UNC\\server\\share for network paths"
		L"\nfindp.exe \\\\?\\c:\\windows"
	);
}