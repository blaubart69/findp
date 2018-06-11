#include "stdafx.h"

void PrintEntry(LPCWSTR FullBaseDir, WIN32_FIND_DATA *finddata)
{
	Log *logger = Log::Instance();
	logger->writeLine(L"%s\\%s", FullBaseDir, finddata->cFileName);
}