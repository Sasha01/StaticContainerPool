
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "SCP.h"

static void SCPTests_testQueue(void);
static void SCPTests_testQueueCreate(void);
static void SCPTests_testQueuePushPop(void);
static void SCPTests_testQueueIsEmpty(void);
static void SCPTests_testQueueIsFull(void);
static void SCPTests_testQueueVarious(void);


static void SCPTests_testQueueDelete(void);

void SCPTests_run(void)
{
    #if SCP_ENABLE_API_QUEUE 
    SCPTests_testQueue();
    #endif  /* SCP_ENABLE_API_QUEUE */
    printf("Running SCP tests! Success!\n");
}

static void SCPTests_testQueue(void)
{
    SCP_init();
    SCPTests_testQueueCreate();
    SCPTests_testQueuePushPop();
    SCPTests_testQueueIsEmpty();
    SCPTests_testQueueIsFull();
    SCPTests_testQueueVarious();
    SCPTests_testQueueDelete();
    printf("Testing queues. Succes!!!\n");
}

static void SCPTests_testQueueCreate(void)
{
    SCPContainerId q1, q2, q3, q4;
    SCPStatus status;
    /* Test queue creation. */
    /* Try to create a queue with 0 elements */
    q1 = SCPQueue_create(0, 4);
    assert(q1 == SCP_INVALID);
    /* Try to create a queue with elements of size 0. */
    q1 = SCPQueue_create(4, 0);
    assert(q1 == SCP_INVALID);
    /* Now create several queues, until there is no more space. */
    q1 = SCPQueue_create(3,2);
    assert(q1 != SCP_INVALID);
    q2 = SCPQueue_create(3,2);
    assert(q2 != SCP_INVALID);
    q3 = SCPQueue_create(100,8);
    assert(q3 != SCP_INVALID);
    /* Assuming the size of the buffer is 1000 bytes, this next queue creation is expected to fail due to lack of space. */
    q4 = SCPQueue_create(10,10);
    assert(q4 == SCP_INVALID);

    /* clean-up: delete the queues in reverse order of creation. */
    status = SCPQueue_delete(q3);
    assert(status == SCPStatus_success);
    status = SCPQueue_delete(q2);
    assert(status == SCPStatus_success);   
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);   


    /* Create the maximum number of containers. */
    SCPContainerId ids[SCP_MAX_NO_OF_CONTAINERS] = {0};
    SCPUWord id;

    for (id = 0; id < SCP_MAX_NO_OF_CONTAINERS; id++)
    {
        ids[id] = SCPQueue_create(10,1);
        assert(ids[id] != SCP_INVALID);
    }
    /* Try to create one more. It should fail. */
    q1 = SCPQueue_create(1,1);
    assert(q1 == SCP_INVALID);

    /* Now delete a random queue and try to re-create it. It should work. */
    status = SCPQueue_delete(ids[4]);
    assert(status == SCPStatus_success);
    ids[4] = SCPQueue_create(10, 1);
    assert(ids[4] != SCP_INVALID);

    /* Delete the same queue again and try to create it with a larger size. It should fail. */
    status = SCPQueue_delete(ids[4]);
    assert(status == SCPStatus_success);
    ids[4] = SCPQueue_create(11, 1);
    assert(ids[4] == SCP_INVALID);

    /* At this point there is a gap of 32 + 10 bytes in the middle of the buffer. */
    /* Now try to create a smaller queue. It should fail as well, because the condition 
    to create a smaller queue over a deleted one is that we should be able to create another 
    empty queue with at least one byte in the remaining space. Now there are 2 reasons why this
    can't be done at this point:
    1. We already created the maximum allowed number of containers and deleted one, so we can only create 
    one more.
    2. There are only 42 bytes available where the deleted queue was, and the container metadata itself is 32 bytes, so in order
    to create 2 queues we need at least 32 * 2 + 2 = 66 bytes (for containers with 1 byte of data, which is 
    the smallest possible).
    For this test only the first reason is valid. If we had more space in the container map, the queue would 
    be created after the existing queues, not in the empty space.
    */
    ids[4] = SCPQueue_create(1, 1);
    assert(ids[4] == SCP_INVALID);
    
    /* clean-up */
    SCP_init();

    /* Create 3 queues of 300 bytes each. 
    This should occupy 300 * 3 + 32 * 3 = 996 bytes of the 1000 bytes allocated to the buffer.
    */
    ids[0] = SCPQueue_create(300, 1);
    assert(ids[0] != SCP_INVALID);
    ids[1] = SCPQueue_create(300, 1);
    assert(ids[1] != SCP_INVALID);
    ids[2] = SCPQueue_create(300, 1);
    assert(ids[2] != SCP_INVALID);

    /* Delete the middle queue. This should free up 332 bytes*/
    status = SCPQueue_delete(ids[1]);
    assert(status == SCPStatus_success);

    /* Create 2 queues of 100 bytes each. This should take 100 * 2 + 32 * 2 = 264 bytes so they should be 
    created in the space freed up by the previous deletion. 
    There will be a leftover free container of 332 - 264 - 32 = 36 bytes*/
    ids[4] = SCPQueue_create(100, 1);
    assert(ids[4] != SCP_INVALID);
    ids[5] = SCPQueue_create(100, 1);
    assert(ids[5] != SCP_INVALID);

    /* Try to create one more container of 37 bytes. This should fail, there is no more space. */
    ids[6] = SCPQueue_create(37, 1);
    assert(ids[6] == SCP_INVALID);

    /* Try to create one more container of 5 bytes. This should fail, there is no more space to 
    create another empty container. */
    ids[6] = SCPQueue_create(5, 1);
    assert(ids[6] == SCP_INVALID);

    /* Try to create one more container of 36 bytes. This should succeed as it fits exactly over 
    the free container. */
    ids[6] = SCPQueue_create(36, 1);
    assert(ids[6] != SCP_INVALID);

    /* clean-up */
    SCP_init();
}

static void SCPTests_testQueueDelete(void)
{
    SCPStatus status = SCPStatus_failed;
    /* create a queue. */
    SCPContainerId q1 = SCPQueue_create(1, 5);
    assert(q1 != SCP_INVALID);
    /* now delete it, it should work. */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
    /* try to delete the queue again... this should fail. */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_failed);
    /* try to delete a NULL queue */
    status = SCPQueue_delete(SCP_INVALID);
    assert(status == SCPStatus_failed);
    /* try to delete a queue with a random id. it should fail */
    status = SCPQueue_delete(5);
    assert(status == SCPStatus_failed);
    /* try to delete a queue with the id greater than the size of the map. it should fail */
    status = SCPQueue_delete(12345);
    assert(status == SCPStatus_failed);


    /* create 2 queues, and try to delete the first one. It should succeed. */
    q1 = SCPQueue_create(1, 5);
    SCPContainerId q2 = SCPQueue_create(3, 4);
    assert(q1 != SCP_INVALID);
    assert(q2 != SCP_INVALID);
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
    /* now delete the second one, it should work. */
    status = SCPQueue_delete(q2);
    assert(status == SCPStatus_success);
    /* now delete the first one, it should fail because it was deleted before. */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_failed);

    /* Now create multiple queues. Try to delete the first one and re-create it, it should work. */
    SCPContainerId q3, q4;
    q1 = SCPQueue_create(3,2);
    assert(q1 != SCP_INVALID);
    q2 = SCPQueue_create(3,2);
    assert(q2 != SCP_INVALID);
    q3 = SCPQueue_create(100,8);
    assert(q3 != SCP_INVALID);
    /* Assuming the size of the buffer is 1000 bytes, this next queue creation is expected to fail due to lack of space. */
    q4 = SCPQueue_create(10,10);
    assert(q4 == SCP_INVALID);
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
    q1 = SCP_INVALID;
    q1 = SCPQueue_create(3,2);
    assert(q1 != SCP_INVALID);
    /* Now delete q1 and q2 and try to create a queue that takes more than q1. */
    status = SCPQueue_delete(q2);
    assert(status == SCPStatus_success);
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
    /* We just freed up 2 * (32 + 3 * 2) = 76 bytes, so we should be able to create a queue with 76 - 32 = 44 bytes of data */

    q1 = SCPQueue_create(11,4);
    assert(q1 != SCP_INVALID);
    /* clean-up */
    SCP_init();
}

static void SCPTests_testQueuePushPop(void)
{
    SCPContainerId q1;
    SCPStatus status;
    const uint16_t q1val1 = 0x1234, q1val2 = 0x5678, q1val3 = 0x9021, q1val4 = 0x1010;
    uint16_t q1out;
    
    /* create queue*/
    q1 = SCPQueue_create(3, 2);
    assert (q1 != SCP_INVALID);

    /* now push an element and pop it. The result should be identical. */
    status = SCPQueue_push(q1, (SCPAddr)&q1val1);
    assert(status == SCPStatus_success);
    status = SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(status == SCPStatus_success);
    assert(q1out == q1val1);

    /* fill up the queue. */
    status = SCPQueue_push(q1, (SCPAddr)&q1val1);
    assert(status == SCPStatus_success); 
    status = SCPQueue_push(q1, (SCPAddr)&q1val2);
    assert(status == SCPStatus_success);
    status = SCPQueue_push(q1, (SCPAddr)&q1val3);
    assert(status == SCPStatus_success);  
    status = SCPQueue_push(q1, (SCPAddr)&q1val4);
    assert(status == SCPStatus_failed);

    /* now pop one element, then we should be able to push one more.*/
    status = SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(status == SCPStatus_success);
    assert(q1out == q1val1);
    status = SCPQueue_push(q1, (SCPAddr)&q1val4);
    assert(status == SCPStatus_success); 

    /* Now pop the rest of the elements from the queue. */
    status = SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(status == SCPStatus_success);
    assert(q1out == q1val2);
    status = SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(status == SCPStatus_success);
    assert(q1out == q1val3);
    status = SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(status == SCPStatus_success);
    assert(q1out == q1val4);

    /* repeat the entire sequence to make sure the wrap-around works. */
    {
        /* now push an element and pop it. The result should be identical. */
        status = SCPQueue_push(q1, (SCPAddr)&q1val1);
        assert(status == SCPStatus_success);
        status = SCPQueue_pop(q1, (SCPAddr)&q1out);
        assert(status == SCPStatus_success);
        assert(q1out == q1val1);

        /* fill up the queue. */
        status = SCPQueue_push(q1, (SCPAddr)&q1val1);
        assert(status == SCPStatus_success); 
        status = SCPQueue_push(q1, (SCPAddr)&q1val2);
        assert(status == SCPStatus_success);
        status = SCPQueue_push(q1, (SCPAddr)&q1val3);
        assert(status == SCPStatus_success);  
        status = SCPQueue_push(q1, (SCPAddr)&q1val4);
        assert(status == SCPStatus_failed);

        /* now pop one element, then we should be able to push one more.*/
        status = SCPQueue_pop(q1, (SCPAddr)&q1out);
        assert(status == SCPStatus_success);
        assert(q1out == q1val1);
        status = SCPQueue_push(q1, (SCPAddr)&q1val4);
        assert(status == SCPStatus_success); 

        /* Now pop the rest of the elements from the queue. */
        status = SCPQueue_pop(q1, (SCPAddr)&q1out);
        assert(status == SCPStatus_success);
        assert(q1out == q1val2);
        status = SCPQueue_pop(q1, (SCPAddr)&q1out);
        assert(status == SCPStatus_success);
        assert(q1out == q1val3);
        status = SCPQueue_pop(q1, (SCPAddr)&q1out);
        assert(status == SCPStatus_success);
        assert(q1out == q1val4);
    }

    /* now try to push a null element. */
    status = SCPQueue_push(q1, NULL);
    assert(status == SCPStatus_failed);
    /* push into a NULL queue */
    status = SCPQueue_push(SCP_INVALID, (SCPAddr)&q1val1);
    assert(status == SCPStatus_failed);
    /* try to pop from an empty queue */
    assert(SCPQueue_isEmpty(q1));
    status = SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(status == SCPStatus_failed);
    /*push one element and try to pop into a NULL address */
    status = SCPQueue_push(q1, (SCPAddr)&q1val4);
    assert(status == SCPStatus_success);
    status = SCPQueue_pop(q1, NULL);
    assert(status == SCPStatus_failed);
    /* now try to pop from a NULL queue. */
    status = SCPQueue_pop(SCP_INVALID, (SCPAddr)&q1out);
    assert(status == SCPStatus_failed);

    /* clean-up */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);

    /* try to push into a deleted queue */
    status = SCPQueue_push(q1, (SCPAddr)&q1val4);
    assert(status == SCPStatus_failed);
    /* try to pop from a deleted queue */
    status = SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(status == SCPStatus_failed);
}

static void SCPTests_testQueueIsEmpty(void)
{
    SCPStatus status;
    SCPContainerId q1;
    const uint16_t q1val = 0x1234;
    uint16_t q1out;
    q1 = SCPQueue_create(3, 2);
    /* the queue was just created, it should be empty */
    assert(SCPQueue_isEmpty(q1) == SCPBool_true);
    /* push an element, now it's not empty anymore */
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);
    assert(SCPQueue_isEmpty(q1) == SCPBool_false);
    /* pop one element, the queue should again be empty */
    (void) SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(SCPQueue_isEmpty(q1) == SCPBool_true);
    /* check the wrap-around: add 2 elements, pop one, fill-up the queue, then empty it. */
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);
    (void) SCPQueue_pop(q1, (SCPAddr)&q1out);
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);    
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);
    assert(SCPQueue_isEmpty(q1) == SCPBool_false);  /* here the queue is actually full */
    (void) SCPQueue_pop(q1, (SCPAddr)&q1out);
    (void) SCPQueue_pop(q1, (SCPAddr)&q1out);
    (void) SCPQueue_pop(q1, (SCPAddr)&q1out);
    assert(SCPQueue_isEmpty(q1) == SCPBool_true);

    /* check that an invalid queue is empty */
    assert(SCPQueue_isEmpty(SCP_INVALID) == SCPBool_true);

    /* add some values in the queue to check if after deletion the queue is empty */
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);    
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);
    assert(SCPQueue_isEmpty(q1) == SCPBool_false);

    /* clean-up */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
    /* a deleted queue should be empty */
    assert(SCPQueue_isEmpty(q1) == SCPBool_true);
}

static void SCPTests_testQueueIsFull(void)
{
    SCPStatus status;
    SCPContainerId q1;
    const uint16_t q1val = 0x1234;
    q1 = SCPQueue_create(3, 2);

    /* the queue was just created, it shouldn't be full */
    assert(SCPQueue_isFull(q1) == SCPBool_false);

    /* add 2 elements, the queue should not be full */
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);  
    assert(SCPQueue_isFull(q1) == SCPBool_false);

    /* add one more element; now the queue is full */
    (void)SCPQueue_push(q1, (SCPAddr)&q1val);  
    assert(SCPQueue_isFull(q1) == SCPBool_true);    

    /* check that an invalid queue is not full */
    assert(SCPQueue_isFull(SCP_INVALID) == SCPBool_false);

    /* clean-up */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
    /* after deletion the queue should not be full */
    assert(SCPQueue_isFull(q1) == SCPBool_false);
}

static void SCPTests_testQueueVarious(void)
{
    SCPContainerId q1 = SCPQueue_create(3,2);
    uint16_t q1val1 = 1234, q1val2 = 4321, q1val3 = 9876, q1val4 = 4567;
    SCPStatus status;
    /* Test queue push. */
    status = SCPQueue_push(q1, (SCPAddr)&q1val1);
    assert(status == SCPStatus_success);
    status = SCPQueue_push(q1, (SCPAddr)&q1val2);
    assert(status == SCPStatus_success);
    assert(SCPQueue_isFull(q1) == 0);
    status = SCPQueue_push(q1, (SCPAddr)&q1val3);
    assert(status == SCPStatus_success);
    assert(SCPQueue_isFull(q1) == 1);
    status = SCPQueue_push(q1, (SCPAddr)&q1val4);
    assert(status == SCPStatus_failed);
    uint16_t output = 0;
    assert(SCPQueue_isFull(q1) == 1);
    /* Test queue pop. */
    status = SCPQueue_pop(q1, (SCPAddr)&output);
    assert(status == SCPStatus_success);
    assert(output == q1val1);
    assert(SCPQueue_isFull(q1) == 0);
    assert(SCPQueue_isEmpty(q1) == 0);
    status = SCPQueue_pop(q1, (SCPAddr)&output);
    assert(status == SCPStatus_success);
    assert(output == q1val2);
    status = SCPQueue_pop(q1, (SCPAddr)&output);
    assert(status == SCPStatus_success);
    assert(output == q1val3);
    assert(SCPQueue_isEmpty(q1) == 1);
    status = SCPQueue_pop(q1, (SCPAddr)&output);
    assert(status == SCPStatus_failed);

    /* Test wrap-around. */
    status = SCPQueue_push(q1, (SCPAddr)&q1val1);
    assert(SCPQueue_getCount(q1) == 1);
    status = SCPQueue_push(q1, (SCPAddr)&q1val2);
    assert(SCPQueue_getCount(q1) == 2);
    status = SCPQueue_push(q1, (SCPAddr)&q1val3);
    assert(SCPQueue_getCount(q1) == 3);
    status = SCPQueue_pop(q1, (SCPAddr)&output);
    assert(output == q1val1);
    assert(SCPQueue_getCount(q1) == 2);
    status = SCPQueue_push(q1, (SCPAddr)&q1val4);
    assert(SCPQueue_getCount(q1) == 3);
    status = SCPQueue_pop(q1, (SCPAddr)&output);
    assert(output == q1val2);    
    assert(SCPQueue_getCount(q1) == 2);
    status = SCPQueue_pop(q1, (SCPAddr)&output);
    assert(output == q1val3);
    assert(SCPQueue_getCount(q1) == 1);
    status = SCPQueue_pop(q1, (SCPAddr)&output);
    assert(output == q1val4);
    assert(SCPQueue_getCount(q1) == 0);

    /* clean-up */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
}
