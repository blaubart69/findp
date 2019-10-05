#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT
#include <windows.h>

#include "beeQueue.h"

#undef RtlMoveMemory
__declspec(dllimport)
void __stdcall RtlMoveMemory(_Out_ void *dst, _In_ const void *src, _In_ size_t len);

BQ* bqInit(int QueueInitLength)
{
    BQ* newQueue = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(BQ));

    if (newQueue == NULL) {
        return NULL;
    }
    newQueue->capacity = QueueInitLength;
    newQueue->initSize = QueueInitLength;

    newQueue->queue = HeapAlloc(GetProcessHeap(), 0, sizeof(void*) * QueueInitLength);

    InitializeCriticalSection(&newQueue->critSec);

    newQueue->semaphoreDequeue = CreateSemaphoreW(NULL, 0, MAXLONG, NULL);

    return newQueue;
}

BOOL bqFree(BQ *bq)
{
    DeleteCriticalSection(&(bq->critSec));
    CloseHandle(bq->semaphoreDequeue);

    BOOL freeData  = HeapFree(GetProcessHeap(), 0, bq->queue);
    BOOL freeQueue = HeapFree(GetProcessHeap(), 0, bq);

    return freeData & freeQueue;
}
// ----------------------------------------------------------------------------
//  case 1: writeIdx > readIdx  || case 2.1 and 2.2: writeIdx <= readIdx
//                              ||
//           writeIdx           ||   writeIdx                     writeIdx
//              |               ||      |                            |
//  _ _ _ + + + _ _ _           ||  + + _ _ _ _ + + +    + + + + + + + + +
//  0 1 2 3 4 5 6 7 8           ||  0 1 2 3 4 5 6 7 8    0 1 2 3 4 5 6 7 8
//        |                     ||              |                    |
//     readIdx                  ||           readIdx              readIdx
// 
// ALL cases: append more memory to the buffer
// 
// case 1:
//  WRITE will have more space. that's all.
//
// case 2.1 and 2.2:
//  we move the elements from readIdx to "end"
//  to the "new end"
//
// ----------------------------------------------------------------------------
BOOL EnlargeQueue(_Inout_ BQ *bq, _In_ const int additionalElements)
{
    const long newCapacity = bq->capacity + additionalElements;

    void *newmem = HeapReAlloc(GetProcessHeap(), 0, bq->queue, newCapacity * sizeof(void*));
    if ( newmem == NULL) {
        return FALSE;
    }

    bq->queue = newmem;

    if (bq->writeIdx <= bq->readIdx) {

        int ElementsToMove = bq->capacity - bq->readIdx;

        int  newReadIdx = newCapacity - ElementsToMove;
        void *src = bq->queue + bq->readIdx;
        void *dst = bq->queue + newReadIdx;

        RtlMoveMemory(dst, src, ElementsToMove * sizeof(void*));

        bq->readIdx = newReadIdx;
    }

    bq->capacity = newCapacity;

    return TRUE;
}

#pragma warning( push )
#pragma warning( disable : 4090) // C4090: '=': different 'const' qualifiers
BOOL bqEnqueue(_Inout_ BQ *bq, _In_ const void * data)
{
    BOOL ok = TRUE;

    EnterCriticalSection(&(bq->critSec));
    {
        if (bq->count == bq->capacity)
        {
            ok = EnlargeQueue(bq, bq->initSize);
        }

        if (ok) {
            bq->queue[bq->writeIdx] = data;
            bq->writeIdx = (bq->writeIdx + 1) % bq->capacity;
            bq->count += 1;
        }
    }
    LeaveCriticalSection(&(bq->critSec));

    long prevCount;
    ReleaseSemaphore(bq->semaphoreDequeue, 1, &prevCount);

    return ok;
}
#pragma warning( pop ) 

_Success_(return)
static BOOL _internal_tryDequeueItemSafe(_Inout_ BQ *bq, _Out_ void **data)
{
    EnterCriticalSection(&(bq->critSec));

    BOOL itemDequeued;
    {
        if (bq->count > 0) {
            *data = bq->queue[bq->readIdx];
            bq->readIdx = (bq->readIdx + 1) % bq->capacity;
            bq->count -= 1;
            itemDequeued = TRUE;
        }
        else {
            itemDequeued = FALSE;
        }
    }
    LeaveCriticalSection(&(bq->critSec));

    return itemDequeued;
}

_Success_(return)
BOOL bqTryDequeue(_Inout_ BQ *bq, _Out_ void **data, _In_ DWORD  dwMilliseconds)
{
    BOOL itemDequeued;

    if (WaitForSingleObject(bq->semaphoreDequeue, dwMilliseconds) == WAIT_OBJECT_0) {
        itemDequeued = _internal_tryDequeueItemSafe(bq, data);
        itemDequeued = TRUE;
    }
    else {
        itemDequeued = FALSE;
    }

    return itemDequeued;
}
