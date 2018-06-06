#include "stdafx.h"

#include "findp.h"
#include "Log.h"

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

	 LPWSTR regex = NULL;

	 for (int i = 1; i < argc; i++)
	 {
		 if (argv[i][0] == L'-' && lstrlen(argv[i]) > 1)
		 {
			 switch (argv[i][1])
			 {
				default:									break;
				 case L's': opts->sum = true;				break;
				 case L'p': opts->progress = true;			break;
				 case L'j': opts->followJunctions = true;	break;
				 case L'd': opts->maxDepth = atoi((const char*)argv[++i]); break;
				 case L'r': regex = argv[++i];				break;
				 case L't': opts->ThreadsToUse = atoi((const char *)argv[++i]); break;
			 }
		 }
		 else
		 {
			 opts->rootDir.assign(argv[i]);
		 }
	 }

	 if (opts->rootDir.empty())
	 {
		 Log::Instance()->err(L"no directory given");;
		 Log::Instance()->inf(L"usage: findp.exe [-s] [-p] [-j] [-d maxDepth] [-r regex-for-filename] {directory}");;
		 return 2;
	 }

	 if (regex != NULL)
	 {
		 Log::Instance()->inf(L"regex parsed: %s", regex);
		 opts->FilenameRegex = std::make_unique<std::wregex>(regex, std::regex_constants::icase);
	 }

	 return 0;
}