#include "stdafx.h"
#include "findp.h"
#include "Log.h"

void ProcessEntry(const std::wstring *FullBaseDir, WIN32_FIND_DATA *finddata, Context *ctx)
{
	Log *logger = Log::Instance();

	bool printEntry = true;
	if (ctx->opts.FilenameRegex != nullptr)
	{
		printEntry = std::regex_search(
			finddata->cFileName
			, *ctx->opts.FilenameRegex.get());
	}

	if (printEntry)
	{
		logger->writeLine(L"%s\\%s", FullBaseDir->c_str(), finddata->cFileName);
	}
}
