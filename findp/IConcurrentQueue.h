#pragma once

template<class T>
class IConcurrentQueue 
{
public:
	virtual ~IConcurrentQueue() {}
	virtual bool enqueue(const T* data)										= 0;
	virtual bool tryDequeue(T** data, const unsigned long milliseconds)		= 0;
};