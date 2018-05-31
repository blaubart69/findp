// findp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "IConcurrentQueue.h"
#include "IOCPQueueImpl.h"
#include "ParallelExec.h"

struct fake {
	int x;
	fake(int value) : x(value) {}
};

void threadFunc(fake *item, ParallelExec<fake>::enqueueItemFunc enqueueNewItem)
{

}

int main()
{
	HANDLE quitPressed;
	quitPressed = CreateEvent(NULL, TRUE, FALSE, NULL);

	IConcurrentQueue<fake> *queue = new IOCPQueueImpl<fake>();
	ParallelExec<fake> *executor = new ParallelExec<fake>(queue, threadFunc, quitPressed, 32);

	const fake *item = new fake(17);
	executor->EnqueueWork(item);
	executor->EnqueueWork(new fake(18));

    return 0;
}

