#pragma once

template<typename T, typename C>
class ParallelExec
{
public:

	typedef void (*WorkFunc)		(T* item, ParallelExec<T,C> *executor, C* context);

	ParallelExec(IConcurrentQueue<T> *queueToUse, WorkFunc workFunc, C* context, int maxThreads);
	~ParallelExec();

	void EnqueueWork(const T* item);
	bool Wait(int milliSeconds) const;
	void Cancel();
	void Stats(long *queued, long *running, long *done) const;

private:

	__declspec(align(64)) volatile long _itemCount;
	__declspec(align(64)) volatile long _running;
	__declspec(align(64)) volatile long _done;

	volatile bool _canceled;

	IConcurrentQueue<T>*  _queue;
	WorkFunc _workFunc;
	C*	_context;
	HANDLE	_hasFinished;
	int		_maxThreads;

	static DWORD WINAPI PoolThread(LPVOID lpParam);
	void StartPoolThreads(int numberToStart);
	void SignalEndToOtherThreads();
};

template<typename T, typename C>
ParallelExec<T, C>::ParallelExec(IConcurrentQueue<T> *queueToUse, WorkFunc workFunc, C* context, int maxThreads)
{
	_queue = queueToUse;
	_workFunc = workFunc;
	_itemCount = _running = _done = 0;
	_canceled = false;
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
DWORD WINAPI ParallelExec<T,C>::PoolThread(LPVOID lpParam)
{
	ParallelExec<T,C> *self = (ParallelExec<T,C>*)lpParam;

	T* item;
	while (self->_queue->tryDequeue(&item, INFINITE))
	{
		if (item == nullptr)
		{
			break;
		}

		InterlockedIncrement(&self->_running);
		self->_workFunc(item, self, self->_context);
		InterlockedDecrement(&self->_running);
		InterlockedIncrement(&self->_done);

		if ( self->_canceled || (InterlockedDecrement(&self->_itemCount) == 0) )
		{
			self->SignalEndToOtherThreads();
			SetEvent(self->_hasFinished);
			break;
		}
	}

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
			//Log::Instance()->dbg(L"thread started. ThreadId: %ld", dwThreadId);
			CloseHandle(hThread);
		}
		
	}
}

template<typename T, typename C>
inline void ParallelExec<T, C>::Cancel()
{
	_canceled = true;
}

template<typename T, typename C>
inline void ParallelExec<T, C>::Stats(long *queued, long *running, long *done) const
{
	*queued = _itemCount;
	*running = _running;
	*done = _done;
}

template<typename T, typename C>
inline void ParallelExec<T, C>::SignalEndToOtherThreads()
{
	for (int i = 0; i < _maxThreads; i++)
	{
		_queue->enqueue(nullptr);
	}
}
