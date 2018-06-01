#pragma once

#include "IConcurrentQueue.h"

template<typename T>
class IOCPQueueImpl : public IConcurrentQueue<T>
{
public:
	IOCPQueueImpl();
	~IOCPQueueImpl()											override;
	bool enqueue(const T* data)									override;
	bool tryDequeue(T** data, const unsigned long milliseconds) override;
	unsigned long count() const									override;
private:

	HANDLE			_hCompletionPort;
	volatile long	_count;
};

//-------------------------------------------------------------------------------------------------
template<typename T>
IOCPQueueImpl<T>::IOCPQueueImpl()
//-------------------------------------------------------------------------------------------------
{
	_count = 0;

	if ((_hCompletionPort = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE
		, NULL
		, 0
		, 0)) == NULL)
	{
	}

}
//-------------------------------------------------------------------------------------------------
template<typename T>
IOCPQueueImpl<T>::~IOCPQueueImpl()
//-------------------------------------------------------------------------------------------------
{
	CloseHandle(_hCompletionPort);
}
//-------------------------------------------------------------------------------------------------
template<typename T>
bool IOCPQueueImpl<T>::enqueue(const T * data)
//-------------------------------------------------------------------------------------------------
{
	BOOL bSuccess = PostQueuedCompletionStatus(
		_hCompletionPort
		, 0
		, (ULONG_PTR)data
		, NULL);

	if (bSuccess)
	{
		InterlockedIncrement(&_count);
	}

	return bSuccess;
}
//-------------------------------------------------------------------------------------------------
template<typename T>
bool IOCPQueueImpl<T>::tryDequeue(T ** data, const unsigned long milliseconds)
//-------------------------------------------------------------------------------------------------
{
	DWORD dwSize;
	LPOVERLAPPED pOverlapped;

	BOOL bSuccess = GetQueuedCompletionStatus(
		_hCompletionPort
		, &dwSize
		, (PULONG_PTR)data
		, &pOverlapped
		, milliseconds);

	if (bSuccess)
	{
		InterlockedDecrement(&_count);
	}

	return bSuccess;
}
//-------------------------------------------------------------------------------------------------
template<typename T>
unsigned long IOCPQueueImpl<T>::count() const
//-------------------------------------------------------------------------------------------------
{
	return _count;
}
