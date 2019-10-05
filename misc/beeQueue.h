#pragma once

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _bq
{
	long				readIdx;
	long				writeIdx;
	long				count;
	long				capacity;
	long				initSize;
	CRITICAL_SECTION	critSec;
	HANDLE				semaphoreDequeue;
	void				**queue;
} BQ;

BQ* bqInit(int QueueInitLength);
BOOL bqFree(BQ* bq);

BOOL bqEnqueue   (_Inout_ BQ *bq, _In_ const void *data);
BOOL bqTryDequeue(_Inout_ BQ *bq, _Out_      void **data, _In_ DWORD  dwMilliseconds);
BOOL EnlargeQueue(_Inout_ BQ *bq, _In_ const int additionalElements);

#ifdef __cplusplus
}
#endif