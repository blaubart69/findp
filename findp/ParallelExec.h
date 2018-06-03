#pragma once

#include "stdafx.h"

#include "IConcurrentQueue.h"

template<typename T, typename C>
class ParallelExec
{
public:

	//typedef void (*enqueueItemFunc)	(ParallelExec<T> *executor);
	typedef void (*WorkFunc)		(T* item, ParallelExec<T,C> *executor, C* context);

	//ParallelExec(IConcurrentQueue<T> *queueToUse, WorkFunc workFunc, C* context, int maxThreads);
	ParallelExec(std::unique_ptr< IConcurrentQueue<T> > &&queueToUse, WorkFunc workFunc, C* context, int maxThreads);
	~ParallelExec();

	void EnqueueWork(const T* item);
	bool Wait(int milliSeconds) const;
	void Cancel();
	void Stats(long *startedThreads, long *endedThreads);
	void SignalEndToOtherThreads();

private:

	//IConcurrentQueue<T> *_queue;
	std::unique_ptr< IConcurrentQueue<T> > _queue;
	WorkFunc _workFunc;
	C* _context;

	HANDLE _hasFinished;
	int		_maxThreads;
	volatile bool _canceled;

	volatile long _itemCount;
	long _startedThreads;
	long _endedThreads;

	static DWORD WINAPI PoolThread(LPVOID lpParam);

	void StartPoolThreads(int numberToStart);
};

template<typename T, typename C>
//ParallelExec<T,C>::ParallelExec(IConcurrentQueue<T> *queueToUse, WorkFunc workFunc, C* context, int maxThreads)
ParallelExec<T, C>::ParallelExec(std::unique_ptr< IConcurrentQueue<T> > &&queueToUse, WorkFunc workFunc, C* context, int maxThreads)
	: _queue(std::move(queueToUse))
{
	_workFunc = workFunc;
	_itemCount = 0;
	_canceled = false;
	_startedThreads = _endedThreads = 0;
	_maxThreads = maxThreads;
	_context = context;
	_hasFinished = CreateEvent(NULL, TRUE, FALSE, NULL);

	StartPoolThreads(maxThreads);
}

template<typename T, typename C>
ParallelExec<T,C>::~ParallelExec()
{
	CloseHandle(_hasFinished);
}

template<typename T, typename C>
bool ParallelExec<T,C>::Wait(int milliSeconds) const
{
	return WaitForSingleObject(_hasFinished, milliSeconds) == WAIT_OBJECT_0;
}

template<typename T, typename C>
void ParallelExec<T,C>::EnqueueWork(const T *item)
{
	InterlockedIncrement(&_itemCount);
	_queue->enqueue(item);
}

template<typename T, typename C>
inline void ParallelExec<T, C>::Cancel()
{
	_canceled = true;
}

template<typename T, typename C>
inline void ParallelExec<T, C>::Stats(long *startedThreads, long *endedThreads)
{
	*startedThreads = _startedThreads;
	*endedThreads = _endedThreads;
}

template<typename T, typename C>
inline void ParallelExec<T, C>::SignalEndToOtherThreads()
{
	for (int i = 0; i < _maxThreads; i++)
	{
		_queue->enqueue(nullptr);
	}
}

template<typename T, typename C>
DWORD WINAPI ParallelExec<T,C>::PoolThread(LPVOID lpParam)
{
	ParallelExec<T,C> *self = (ParallelExec<T,C>*)lpParam;
	InterlockedIncrement(&self->_startedThreads);

	T* item;
	while (self->_queue->tryDequeue(&item, INFINITE))
	{
		if (item != nullptr)
		{
			self->_workFunc(item, self, self->_context);
			delete item;

			if (InterlockedDecrement(&self->_itemCount) == 0)
			{
				self->SignalEndToOtherThreads();
				SetEvent(self->_hasFinished);
				break;
			}
		}
		else
		{
			break;
		}
	}

	InterlockedIncrement(&self->_endedThreads);

	return 0;
}

template<typename T,typename C>
void ParallelExec<T, C>::StartPoolThreads(int numberToStart)
{
	for (int i = 0; i < numberToStart; i++)
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
			CloseHandle(hThread);
		}
	}
}