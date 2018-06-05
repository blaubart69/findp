#include "stdafx.h"
#include "findp.h"
#include "Log.h"

void ProcessEntry(const std::wstring *FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx)
{
	Log *logger = Log::Instance();

	logger->writeLine(L"%s\\%s", FullBaseDir->c_str(), finddata->cFileName);

}