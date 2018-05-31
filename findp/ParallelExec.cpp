#include "stdafx.h"

#include "ParallelExec.h"

template<typename T>
ParallelExec<T>::ParallelExec(IConcurrentQueue<T> *queueToUse, ThreadFunc workFunc, HANDLE QuitPressed, int maxThreads)
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

	T* item;
	while ( self->_queue->tryDequeue(&item, 0) )
	{
		self->_workFunc(item, self->EnqueueWork);

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