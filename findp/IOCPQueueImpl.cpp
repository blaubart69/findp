#include "stdafx.h"

#include "IOCPQueueImpl.h"

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
	{	}

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
	OVERLAPPED pOverlapped;

	BOOL bSuccess = GetQueuedCompletionStatus(
		_hCompletionPort
		, &dwSize
		, (PULONG_PTR)&data
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
