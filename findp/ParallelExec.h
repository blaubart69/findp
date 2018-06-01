#pragma once

#include "stdafx.h"

#include "IConcurrentQueue.h"

template<typename T, typename C>
class ParallelExec
{
public:

	//typedef void (*enqueueItemFunc)	(ParallelExec<T> *executor);
	typedef void (*WorkFunc)		(T* item, ParallelExec<T,C> *executor, C* context);

	ParallelExec(IConcurrentQueue<T> *queueToUse, WorkFunc workFunc, C* context, HANDLE QuitPressed, int maxThreads);
	~ParallelExec();

	void EnqueueWork(const T* item);
	bool Wait(int milliSeconds);

private:

	IConcurrentQueue<T> *_queue;
	WorkFunc _workFunc;
	C* _context;

	HANDLE _hasFinished;
	HANDLE _QuitPressed;
	int		_maxThreads;

	long _running;
	long _startedThreads;

	static DWORD WINAPI PoolThread(LPVOID lpParam);

	void StartPoolThread();
};

template<typename T, typename C>
ParallelExec<T,C>::ParallelExec(IConcurrentQueue<T> *queueToUse, WorkFunc workFunc, C* context, HANDLE QuitPressed, int maxThreads)
{
	_queue = queueToUse;
	_workFunc = workFunc;
	_running = 0;
	_startedThreads = 0;
	_maxThreads = maxThreads;
	_QuitPressed = QuitPressed;
	_context = context;
	_hasFinished = CreateEvent(NULL, TRUE, FALSE, NULL);
}

template<typename T, typename C>
ParallelExec<T,C>::~ParallelExec()
{
	CloseHandle(_hasFinished);
}

template<typename T, typename C>
bool ParallelExec<T,C>::Wait(int milliSeconds)
{
	return WaitForSingleObject(_hasFinished, milliSeconds) == WAIT_OBJECT_0;
}

template<typename T, typename C>
void ParallelExec<T,C>::EnqueueWork(const T *item)
{
	_queue->enqueue(item);

	if (_running < _maxThreads)
	{
		StartPoolThread();
	}
}

template<typename T, typename C>
DWORD WINAPI ParallelExec<T,C>::PoolThread(LPVOID lpParam)
{
	ParallelExec<T,C> *self = (ParallelExec<T,C>*)lpParam;

	InterlockedIncrement(&self->_running);

	T* item;
	while (self->_queue->tryDequeue(&item, 0))
	{
		self->_workFunc(item, self, self->_context);
		delete item;

		if (WaitForSingleObject(self->_QuitPressed, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}

	if (InterlockedDecrement(&self->_running) == 0)
	{
		SetEvent(self->_hasFinished);
	}

	return 0;
}

template<typename T,typename C>
void ParallelExec<T,C>::StartPoolThread()
{
	DWORD dwThreadId;

	HANDLE hThread = CreateThread(
		NULL
		, 0
		, PoolThread
		, this
		, 0
		, &dwThreadId);
	
	if (hThread != NULL)
	{
		InterlockedIncrement(&_startedThreads);
		CloseHandle(hThread);
	}

}