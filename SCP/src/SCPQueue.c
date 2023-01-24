#include <string.h>
#include "SCPTypes.h"
#include "SCPInternal.h"
#include "SCPQueue.h"


#define QUEUE_FULL_MARKER (1u)
#define QUEUE_TYPE_MARKER (0x12 << 1)

#define IS_QUEUE_FULL(queue)        (((queue)->meta & QUEUE_FULL_MARKER) ? SCPBool_true : SCPBool_false)
#define IS_QUEUE_EMPTY(queue)       ((((queue)->c.q.head == (queue)->c.q.tail) && !IS_QUEUE_FULL(q)) ? SCPBool_true : SCPBool_false)
#define SET_QUEUE_FULL(queue)       ((queue)->meta |= QUEUE_FULL_MARKER)
#define CLEAR_QUEUE_FULL(queue)     ((queue)->meta &= ~QUEUE_FULL_MARKER)
#define END_OF_QUEUE_DATA(queue)    (SCPAddr)((SCPAddr)(queue) + sizeof(SCPContainer) + ((queue)->c.q.maxNoOfElem * (queue)->c.q.sizeOfElem))
#define START_OF_QUEUE_DATA(queue)  (SCPAddr)((SCPAddr)(queue) + sizeof(SCPContainer))
#define IS_QUEUE(queue)             (((queue)->meta & QUEUE_TYPE_MARKER) == QUEUE_TYPE_MARKER ? SCPBool_true : SCPBool_false)
#define SET_QUEUE_TYPE_MARKER(queue)    ((queue)->meta |= QUEUE_TYPE_MARKER)
#define IS_QUEUE_VALID(queue)       (((queue != SCP_NULL) && IS_QUEUE(queue) && \
                                    ((queue)->c.q.head != SCP_NULL) && \
                                    ((queue)->c.q.tail != SCP_NULL) && \
                                    ((queue)->c.q.maxNoOfElem > 0) && \
                                    ((queue)->c.q.sizeOfElem > 0)) ? SCPBool_true : SCPBool_false)



static void createEmptyQueue(SCPContainer** newQueue, const SCPUWord elem, const SCPUWord size);

SCPContainer* SCPQueue_create(const SCPUWord noOfElem, const SCPUWord sizeOfElem)
{
    SCP_ENTER_CRITICAL_SECTION();
    SCPContainer* newQueue = SCP_NULL;
    /* If this is the first to be created, initialize nextFree. */
    if (scp.nextFree == SCP_NULL)
    {
        scp.nextFree = (SCPContainer*)&scp.buffer[0];
    }

    /* check the free space in the buffer */
    SCPUWord neededSpace = sizeof(SCPContainer) + (noOfElem * sizeOfElem);
    SCPUWord freeSpace = SCP_TOTAL_BUFFER_SIZE - ((SCPAddr)scp.nextFree - scp.buffer);
    if (freeSpace >= neededSpace)
    {
        /* there is enough space for the queue, create it */
        createEmptyQueue(&newQueue, noOfElem, sizeOfElem);
    }    

    SCP_EXIT_CRITICAL_SECTION();
    return newQueue;
}

SCPStatus SCPQueue_push(SCPContainer* const q, SCPAddr const data)
{
    SCPStatus status = SCPStatus_failed;
    if (IS_QUEUE_VALID(q) && (data != SCP_NULL) && !IS_QUEUE_FULL(q))
    {
        memcpy(data, q->c.q.head, q->c.q.sizeOfElem);
        // advance the head
        q->c.q.head += q->c.q.sizeOfElem;
        if (q->c.q.head >= END_OF_QUEUE_DATA(q))
        {
            q->c.q.head = START_OF_QUEUE_DATA(q);
        }
        if (q->c.q.head == q->c.q.tail)
        {
            SET_QUEUE_FULL(q);
        }
        status = SCPStatus_success;
    }
    return status;
}

SCPStatus SCPQueue_pop(SCPContainer* const q, SCPAddr data)
{
    SCPStatus status = SCPStatus_failed;
    if (IS_QUEUE_VALID(q) && (data != SCP_NULL) && !IS_QUEUE_EMPTY(q))
    {
        memcpy(data, q->c.q.tail, q->c.q.sizeOfElem);
        // advance the tail
        q->c.q.tail += q->c.q.sizeOfElem;
        if (q->c.q.tail >= END_OF_QUEUE_DATA(q))
        {
            q->c.q.tail = START_OF_QUEUE_DATA(q);
        }
        CLEAR_QUEUE_FULL(q);
        status = SCPStatus_success;
    }
    return status;
}

SCPBool SCPQueue_isEmpty(SCPContainer* const q)
{
    return IS_QUEUE_EMPTY(q);
}

SCPBool SCPQueue_isFull(SCPContainer* const q)
{
    return IS_QUEUE_FULL(q);
}

SCPUWord SCPQueue_getCount(const SCPContainer* const q)
{
    SCPUWord noOfElem;
    if (IS_QUEUE_EMPTY(q))
    {
        noOfElem = 0;
    }
    else if (IS_QUEUE_FULL(q))
    {
        noOfElem = q->c.q.maxNoOfElem;
    }
    else if (q->c.q.head > q->c.q.tail)
    {
        noOfElem = (q->c.q.head - q->c.q.tail) / q->c.q.sizeOfElem;
    }
    else /* q-> head < q-> tail */
    {
        noOfElem = ((q->c.q.head - START_OF_QUEUE_DATA(q)) + (END_OF_QUEUE_DATA(q) - q->c.q.tail)) / q->c.q.sizeOfElem;
    }
    return noOfElem;
}

static void createEmptyQueue(SCPContainer** newQueue, const SCPUWord elem, const SCPUWord size)
{
    (*newQueue) = scp.nextFree;
    (*newQueue)->c.q.maxNoOfElem = elem;
    (*newQueue)->c.q.sizeOfElem = size;
    (*newQueue)->c.q.head = (SCPAddr)*newQueue + sizeof(SCPContainer);
    (*newQueue)->c.q.tail = (*newQueue)->c.q.head;
    SET_QUEUE_TYPE_MARKER(*newQueue);
    scp.nextFree = (SCPContainer*)END_OF_QUEUE_DATA(scp.nextFree);
}
