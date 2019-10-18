#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT
#include <windows.h>

#include "beeThreadQueue.h"

VOID Message(DWORD MsgId, ...);

template<class T>
beeThreadQueue<T>::beeThreadQueue(size_t initialQueueLength)
{
	_capacity = initialQueueLength;
	_initSize = initialQueueLength;

	const size_t queueLengthToAllocate = sizeof(T*) * QueueInitLength;
	if ((_queue = HeapAlloc(GetProcessHeap(), 0, queueLengthToAllocate)) == NULL) {
		Message(MSGID_ERROR_WINAPI_U, TEXT(__FILE__), __LINE__, L"HeapAlloc", queueLengthToAllocate, GetLastError());
	}

	InitializeCriticalSection(&_critSec);
}

template<class T>
beeThreadQueue<T>::~beeThreadQueue()
{
	DeleteCriticalSection(&_critSec);
	HeapFree(GetProcessHeap(), 0, _queue);
}

template<class T>
bool beeThreadQueue<T>::enlargeQueue(size_t additionalElements)
{
	const long newCapacity = _capacity + additionalElements;

	const size_t newSizeInBytes = newCapacity * sizeof(T*);
	T* newmem = HeapReAlloc(GetProcessHeap(), 0, _queue, newSizeInBytes);
	if (newmem == NULL) {
		Message(MSGID_ERROR_WINAPI_U, TEXT(__FILE__), __LINE__, L"HeapReAlloc", newSizeInBytes, GetLastError());
		return FALSE;
	}

	_queue = newmem;

	if (_writeIdx <= _readIdx) {

		const int ElementsToMove = _capacity - _readIdx;

		const int  newReadIdx = newCapacity - ElementsToMove;
		const T *src = _queue + _readIdx;
		const T *dst = _queue + newReadIdx;

		RtlMoveMemory(dst, src, ElementsToMove * sizeof(T*));

		_readIdx = newReadIdx;
	}

	_capacity = newCapacity;

	return true;
}


template<class T>
bool beeThreadQueue<T>::enqueue(const T * data)
{
	bool ok = true;

	EnterCriticalSection(_critSec);
	{
		if (_count == _capacity)
		{
			ok = EnlargeQueue();
		}

		if (ok) {
			_queue[_writeIdx] = data;
			_writeIdx = (_writeIdx + 1) % _capacity;
			_count += 1;
		}
	}
	LeaveCriticalSection(_bq->critSec);

	return ok;
}

template<class T>
bool beeThreadQueue<T>::tryDequeue(T ** data, const unsigned long milliseconds)
{
	EnterCriticalSection(&_critSec);

	bool itemDequeued;
	{
		if (_count > 0) {
			*data = _queue[bq->readIdx];
			_readIdx = (_readIdx + 1) % _capacity;
			_count -= 1;
			itemDequeued = true;
		}
		else {
			itemDequeued = false;
		}
	}
	LeaveCriticalSection(_bq->critSec);

	return itemDequeued;
}
