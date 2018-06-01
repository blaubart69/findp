// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Log.h"

#include "IConcurrentQueue.h"
#include "IOCPQueueImpl.h"
#include "ParallelExec.h"

#include "EnumDir.h"

Log* logger;

struct DirEntry {
public:
	std::unique_ptr<std::wstring> FullDirname;

	DirEntry(std::unique_ptr<std::wstring> dirname) 
		: FullDirname(std::move(dirname))
	{
	}
};

void threadEnumFunc(DirEntry *item, ParallelExec<DirEntry> *executor)
{
	EnumDir(item->FullDirname.get(), 
		[item, executor] (WIN32_FIND_DATA *finddata)
	{
		logger->inf(L"%s\\%s", item->FullDirname.get()->c_str() , finddata->cFileName );

		if ((finddata->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			auto newFullDir = std::make_unique<std::wstring>();
			newFullDir->assign(*(item->FullDirname.get()));
			newFullDir->append(L"\\");
			newFullDir->append(finddata->cFileName);

			executor->EnqueueWork( new DirEntry(newFullDir) );
		}
	});
}

int main()
{
	logger = Log::Instance();

	HANDLE quitPressed;
	quitPressed = CreateEvent(NULL, TRUE, FALSE, NULL);

	auto *queue		= new IOCPQueueImpl<DirEntry>();
	auto *executor  = new ParallelExec<DirEntry>(queue, threadEnumFunc, quitPressed, 32);

	executor->EnqueueWork(new DirEntry(L"c:\\temp"));

	while (! executor->Wait(1000))
	{
		logger->inf(L"wait: ...");
	}
	

    return 0;
}

