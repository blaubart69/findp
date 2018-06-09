#include "stdafx.h"
#include "findp.h"
#include "Log.h"

void PrintEntry(const std::wstring *FullBaseDir, WIN32_FIND_DATA *finddata)
{
	Log *logger = Log::Instance();
	logger->writeLine(L"%s\\%s", FullBaseDir->c_str(), finddata->cFileName);
}