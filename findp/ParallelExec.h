#pragma once

#include "IConcurrentQueue.h"

template<typename T>
class ParallelExec
{
public:

	typedef void (*enqueueItemFunc)	(const T* item);
	typedef void (*ThreadFunc)		(T* item, enqueueItemFunc);

	ParallelExec(IConcurrentQueue<T*> queueToUse, ThreadFunc workFunc, HANDLE QuitPressed, int maxThreads);
	~ParallelExec();

	void Start(const T* item);
	bool Wait(int milliSeconds);

private:

	IConcurrentQueue<T*> *_queue;
	ThreadFunc _workFunc;

	HANDLE _hasFinished;
	HANDLE _QuitPressed;
	int		_maxThreads;

	long _running;
	long _enqueued;

	void EnqueueWork(const T* item);
	static DWORD WINAPI PoolThread(LPVOID lpParam);

	void StartPoolThread();
};

