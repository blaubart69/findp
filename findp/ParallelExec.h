#pragma once

#include "stdafx.h"

#include "IConcurrentQueue.h"

template<typename T>
class ParallelExec
{
public:

	typedef void (*enqueueItemFunc)	(const T* item);
	typedef void (*WorkFunc)		(T* item, enqueueItemFunc enqFunc);

	ParallelExec(IConcurrentQueue<T> *queueToUse, WorkFunc workFunc, HANDLE QuitPressed, int maxThreads);
	~ParallelExec();

	void EnqueueWork(const T* item);
	bool Wait(int milliSeconds);

private:

	IConcurrentQueue<T> *_queue;
	WorkFunc _workFunc;

	HANDLE _hasFinished;
	HANDLE _QuitPressed;
	int		_maxThreads;

	long _running;

	static DWORD WINAPI PoolThread(LPVOID lpParam);

	void StartPoolThread();
};

template<typename T>
ParallelExec<T>::ParallelExec(IConcurrentQueue<T> *queueToUse, WorkFunc workFunc, HANDLE QuitPressed, int maxThreads)
{
	_queue = queueToUse;
	_workFunc = workFunc;
	_running = 0;
	_maxThreads = maxThreads;
	_QuitPressed = QuitPressed;
	_hasFinished = CreateEvent(NULL, TRUE, FALSE, NULL);
}

template<typename T>
ParallelExec<T>::~ParallelExec()
{
	CloseHandle(_hasFinished);
}

template<typename T>
bool ParallelExec<T>::Wait(int milliSeconds)
{
	return WaitForSingleObject(_hasFinished, milliSeconds) == WAIT_OBJECT_0;
}

template<typename T>
void ParallelExec<T>::EnqueueWork(const T *item)
{
	_queue->enqueue(item);

	if (_running < _maxThreads)
	{
		StartPoolThread();
	}
}

template<typename T>
DWORD WINAPI ParallelExec<T>::PoolThread(LPVOID lpParam)
{
	ParallelExec<T> *self = (ParallelExec<T>*)lpParam;

	InterlockedIncrement(&self->_running);

	enqueueItemFunc encFunc = &ParallelExec<T>::EnqueueWork;

	T* item;
	while (self->_queue->tryDequeue(&item, 0))
	{
		//self->_workFunc(item, self->EnqueueWork);
		self->_workFunc(item, encFunc);

		if (WaitForSingleObject(self->_QuitPressed, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}

	if (InterlockedDecrement(&self->_running) == 0)
	{
		SetEvent(self->_hasFinished);
	}
}

template<typename T>
void ParallelExec<T>::StartPoolThread()
{
	DWORD dwThreadId;

	if (CreateThread(
		NULL
		, 0
		, PoolThread
		, this
		, 0
		, &dwThreadId) == NULL)
	{

	}
}