#include "stdafx.h"

#include "findp.h"
#include "Log.h"

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
	 opts->FilenameRegex = std::unique_ptr<std::wregex>{};
	 opts->ThreadsToUse = 32;
	 opts->SumUpExtensions = false;
	 opts->matchByRegEx = false;

	 LPWSTR regex = NULL;
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
				 case L'r': if ( i+1 < argc) regex = argv[++i];				break;
				 case L'd': if ( i+1 < argc) opts->maxDepth     = _wtoi((const wchar_t*)argv[++i]); break;
				 case L't': if ( i+1 < argc) opts->ThreadsToUse = _wtoi((const wchar_t*)argv[++i]); break;
			 }
		 }
		 else
		 {
			 opts->rootDir.assign(argv[i]);
		 }
	 }

	 if (showHelp)
	 {
		 PrintUsage();
		 return 4;
	 }

	 if (opts->rootDir.empty())
	 {
		 Log::Instance()->err(L"no directory given");;
		 PrintUsage();
		 return 2;
	 }

	 if (regex != NULL)
	 {
		 Log::Instance()->dbg(L"regex parsed: %s", regex);
		 opts->FilenameRegex = std::make_unique<std::wregex>(regex, std::regex_constants::icase);
		 opts->matchByRegEx = true;
	 }

	 return 0;
}
void PrintUsage(void)
{
	Log::Instance()->inf(
		L"usage: findp.exe [OPTIONS] {directory}"
		L"\n-s ... sum dirs, files, filesize. don't print anything"
		L"\n-p ... show progress"
		L"\n-j ... follow directory junctions"
		L"\n-r ... regex to match filenames"
		L"\n-d ... how many directory to go down"
		L"\n-v ... verbose/debug"
		L"\n-h ... show this help"
	    L"\n\nSamples:"
		L"\nfindp.exe -r \"\\.pdf$\" ... find all files with pdf extension"
		L"\n"
		L"\nprepend   \\\\?\\ if you want to have long path support."
		L"\nOr        \\\\?\\UNC\\server\\share for network paths"
		L"\nfindp.exe \\\\?\\c:\\windows"
	);
}