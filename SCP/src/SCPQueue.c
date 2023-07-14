#include <string.h>
#include "SCPTypes.h"
#include "SCPInternal.h"
#include "SCPQueue.h"

#define QUEUE_TYPE_MARKER (0x12)

#define IS_QUEUE_EMPTY(queue)       (((queue)->c.q.head == (queue)->c.q.tail) && !IS_CONTAINER_FULL(queue))

#define SET_QUEUE_TYPE_MARKER(queue)  SET_CONTAINER_TYPE_MARKER((queue), (QUEUE_TYPE_MARKER))

#define IS_QUEUE(queue)             (GET_CONTAINER_TYPE_MARKER(queue) == QUEUE_TYPE_MARKER)

#define IS_ADDR_IN_QUEUE_RANGE(addr, queue) (((SCPAddr)(addr) >= START_OF_CONTAINER_DATA(queue)) && \
                                                ((SCPAddr)(addr) < END_OF_CONTAINER_DATA(queue)))

#define IS_QUEUE_VALID(queue)       ((IS_ADDR_IN_BUFFER_RANGE(queue) && \
                                    IS_QUEUE(queue) && \
                                    IS_ADDR_IN_QUEUE_RANGE((queue)->c.q.head, (queue)) && \
                                    IS_ADDR_IN_QUEUE_RANGE((queue)->c.q.tail, (queue)) && \
                                    ((queue)->maxNoOfElem > 0) && \
                                    ((queue)->sizeOfElem > 0)))


static void initNewQueue(SCPContainer* const newQueue, const SCPUWord elem, const SCPUWord size);

SCPContainerId SCPQueue_create(const SCPUWord noOfElem, const SCPUWord sizeOfElem)
{  
    return SCP_createContainer(noOfElem, sizeOfElem, initNewQueue);
}

SCPStatus SCPQueue_delete(const SCPContainerId id)
{
    SCPStatus status = SCPStatus_failed;
    SCPContainer* const q = SCP_getContainer(id);
    if (IS_QUEUE_VALID(q))
    {
        SCP_freeContainter(id);
        status = SCPStatus_success;
    }
    return status;
}

SCPStatus SCPQueue_push(const SCPContainerId id, SCPAddr const data)
{
    SCPStatus status = SCPStatus_failed;
    SCPContainer* const container = SCP_getContainer(id);
    if (IS_QUEUE_VALID(container) && (data != SCP_NULL) && !IS_CONTAINER_FULL(container))
    {
        memcpy(container->c.q.head, data, container->sizeOfElem);
        // advance the head
        container->c.q.head += container->sizeOfElem;
        if (container->c.q.head >= END_OF_CONTAINER_DATA(container))
        {
            container->c.q.head = START_OF_CONTAINER_DATA(container);
        }
        if (container->c.q.head == container->c.q.tail)
        {
            SET_CONTAINER_FULL(container);
        }
        status = SCPStatus_success;
    }
    return status;
}

SCPStatus SCPQueue_pop(const SCPContainerId id, SCPAddr data)
{
    SCPStatus status = SCPStatus_failed;
    SCPContainer* const container = SCP_getContainer(id);
    if (IS_QUEUE_VALID(container) && (data != SCP_NULL) && !IS_QUEUE_EMPTY(container))
    {
        memcpy(data, container->c.q.tail, container->sizeOfElem);
        // advance the tail
        container->c.q.tail += container->sizeOfElem;
        if (container->c.q.tail >= END_OF_CONTAINER_DATA(container))
        {
            container->c.q.tail = START_OF_CONTAINER_DATA(container);
        }
        CLEAR_CONTAINER_FULL(container);
        status = SCPStatus_success;
    }
    return status;
}

SCPBool SCPQueue_isEmpty(const SCPContainerId id)
{
    SCPBool isEmpty = SCPBool_true;
    SCPContainer* const q = SCP_getContainer(id);
    if (IS_QUEUE_VALID(q))
    {
        isEmpty = IS_QUEUE_EMPTY(q);
    }
    return isEmpty;
}

SCPBool SCPQueue_isFull(const SCPContainerId id)
{
    SCPBool isFull = SCPBool_false;
    SCPContainer* const q = SCP_getContainer(id);
    if (IS_QUEUE_VALID(q))
    {
        isFull = IS_CONTAINER_FULL(q);
    }
    return isFull;
}

SCPUWord SCPQueue_getCount(const SCPContainerId id)
{
    SCPUWord noOfElem;
    SCPContainer* const q = SCP_getContainer(id);
    if (IS_QUEUE_EMPTY(q))
    {
        noOfElem = 0;
    }
    else if (IS_CONTAINER_FULL(q))
    {
        noOfElem = q->maxNoOfElem;
    }
    else if (q->c.q.head > q->c.q.tail)
    {
        noOfElem = (q->c.q.head - q->c.q.tail) / q->sizeOfElem;
    }
    else /* q-> head < q-> tail */
    {
        noOfElem = ((q->c.q.head - START_OF_CONTAINER_DATA(q)) + (END_OF_CONTAINER_DATA(q) - q->c.q.tail)) / q->sizeOfElem;
    }
    return noOfElem;
}

static void initNewQueue(SCPContainer* const newQueue, const SCPUWord elem, const SCPUWord size)
{
    newQueue->maxNoOfElem = elem;
    newQueue->sizeOfElem = size;
    newQueue->c.q.head = (SCPAddr)newQueue + sizeof(SCPContainer);
    newQueue->c.q.tail = newQueue->c.q.head;
    SET_QUEUE_TYPE_MARKER(newQueue);
}