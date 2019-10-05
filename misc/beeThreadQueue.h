#pragma once

#include "IThreadQueue.h"

template<class T>
class beeThreadQueue : public IThreadQueue<T> {
private:
	long				_readIdx;
	long				_writeIdx;
	long				_count;
	long				_capacity;
	long				_initSize;
	CRITICAL_SECTION	_critSec;
	T					**_queue;

	bool enlargeQueue(size_t additionalArguments);

public:
	beeThreadQueue(size_t initialQueueLength);
	~beeThreadQueue() override;

	bool enqueue(const T* data) override;
	bool tryDequeue(T** data, unsigned long milliseconds) override;
};