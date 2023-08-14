#include <string.h>
#include "SCPTypes.h"
#include "SCPInternal.h"
#include "SCPQueue.h"

#define IS_QUEUE_VALID(queue)       (IS_ADDR_IN_BUFFER_RANGE(queue) && \
                                    ((queue)->type == SCPContainerType_queue) && \
                                    IS_ADDR_IN_CONTAINER_RANGE((queue)->c.q.head, (queue)) && \
                                    IS_ADDR_IN_CONTAINER_RANGE((queue)->c.q.tail, (queue)) && \
                                    ((queue)->maxNoOfElem > 0) && \
                                    ((queue)->sizeOfElem > 0))


static void initNewQueue(SCPContainer* const newQueue, const SCPUShort elem, const SCPUShort size);

SCPContainerId SCPQueue_create(const SCPUShort noOfElem, const SCPUShort sizeOfElem)
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
    if (IS_QUEUE_VALID(container) && (data != SCP_NULL) && (container->noOfElem < container->maxNoOfElem))
    {
        memcpy(container->c.q.head, data, container->sizeOfElem);
        // advance the head
        container->c.q.head += container->sizeOfElem;
        if (container->c.q.head >= END_OF_CONTAINER_DATA(container))
        {
            container->c.q.head = START_OF_CONTAINER_DATA(container);
        }
        container->noOfElem++;
        status = SCPStatus_success;
    }
    return status;
}

SCPStatus SCPQueue_pop(const SCPContainerId id, SCPAddr data)
{
    SCPStatus status = SCPStatus_failed;
    SCPContainer* const container = SCP_getContainer(id);
    if (IS_QUEUE_VALID(container) && (data != SCP_NULL) && (container->noOfElem > 0))
    {
        memcpy(data, container->c.q.tail, container->sizeOfElem);
        // advance the tail
        container->c.q.tail += container->sizeOfElem;
        if (container->c.q.tail >= END_OF_CONTAINER_DATA(container))
        {
            container->c.q.tail = START_OF_CONTAINER_DATA(container);
        }
        container->noOfElem--;
        status = SCPStatus_success;
    }
    return status;
}

SCPBool SCPQueue_isEmpty(const SCPContainerId id)
{
    SCPBool isEmpty = SCPBool_true;
    SCPContainer* const q = SCP_getContainer(id);
    if (IS_QUEUE_VALID(q) && (q->noOfElem > 0))
    {
        isEmpty = SCPBool_false;
    }
    return isEmpty;
}

SCPBool SCPQueue_isFull(const SCPContainerId id)
{
    SCPBool isFull = SCPBool_false;
    SCPContainer* const q = SCP_getContainer(id);
    if (IS_QUEUE_VALID(q) && (q->noOfElem >= q->maxNoOfElem))
    {
        isFull = SCPBool_true;
    }
    return isFull;
}

SCPUWord SCPQueue_getCount(const SCPContainerId id)
{
    SCPUWord noOfElem = 0;
    SCPContainer* const q = SCP_getContainer(id);
    if (IS_QUEUE_VALID(q))
    {
        noOfElem = q->noOfElem;
    }
    return noOfElem;
}

static void initNewQueue(SCPContainer* const newQueue, const SCPUShort elem, const SCPUShort size)
{
    newQueue->maxNoOfElem = elem;
    newQueue->sizeOfElem = size;
    newQueue->noOfElem = 0;
    newQueue->c.q.head = (SCPAddr)newQueue + sizeof(SCPContainer);
    newQueue->c.q.tail = newQueue->c.q.head;
    newQueue->type = SCPContainerType_queue;
}