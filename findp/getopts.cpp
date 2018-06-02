#include "stdafx.h"

#include "getopts.h"
#include "Log.h"



int getopts(int argc, wchar_t *argv[], Options* opts)
{
	 if (argc < 2)
	 {
		 return 1;
	 }

	 opts->sum = false;
	 opts->progress = false;

	 for (int i = 1; i < argc; i++)
	 {
		 if (argv[i][0] == L'-' && lstrlen(argv[i]) > 1)
		 {
			 switch (argv[i][1])
			 {
				default:								break;
				 case L's': opts->sum = true;			break;
				 case L'p': opts->progress = true;		break;
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
		 Log::Instance()->inf(L"usage: findp.exe [-s] {directory}");;
		 return 2;
	 }

	 return 0;
}