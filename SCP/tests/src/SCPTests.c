
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "SCP.h"

static void SCPTests_testQueue(void);
static void SCPTests_testQueueCreate(void);
static void SCPTests_testQueueDelete(void);
static void SCPTests_testQueuePushPop(void);
static void SCPTests_testQueueIsEmpty(void);
static void SCPTests_testQueueIsFull(void);

void SCPTests_run(void)
{
    #if SCP_ENABLE_API_QUEUE 
    SCPTests_testQueue();
    #endif  /* SCP_ENABLE_API_QUEUE */
    printf("Running SCP tests! Success!\n");
}

static void SCPTests_testQueue(void)
{
    SCPTests_testQueueCreate();
    SCPTests_testQueuePushPop();
    SCPTests_testQueueIsEmpty();
    SCPTests_testQueueIsFull();
    SCPContainer* q1 = SCPQueue_create(3,2);
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
    SCPTests_testQueueDelete();
    printf("Testing queues. Succes!!!\n");
}

static void SCPTests_testQueueCreate(void)
{
    SCPContainer *q1, *q2, *q3, *q4;
    SCPStatus status;
    /* Test queue creation. */
    /* Try to create a queue with 0 elements */
    q1 = SCPQueue_create(0, 4);
    assert(q1 == NULL);
    /* Try to create a queue with elements of size 0. */
    q1 = SCPQueue_create(4, 0);
    assert(q1 == NULL);
    /* Now create several queues, until there is no more space. */
    q1 = SCPQueue_create(3,2);
    assert(q1 != NULL);
    q2 = SCPQueue_create(3,2);
    assert(q2 != NULL);
    q3 = SCPQueue_create(100,8);
    assert(q3 != NULL);
    /* Assuming the size of the buffer is 1000B, this next queue creation is expected to fail due to lack of space. */
    q4 = SCPQueue_create(10,10);
    assert(q4 == NULL);

    /* clean-up: delete the queues in reverse order of creation. */
    status = SCPQueue_delete(q3);
    assert(status == SCPStatus_success);
    status = SCPQueue_delete(q2);
    assert(status == SCPStatus_success);   
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);   
}

static void SCPTests_testQueueDelete(void)
{
    SCPStatus status = SCPStatus_failed;
    /* create a queue. */
    SCPContainer* q1 = SCPQueue_create(1, 5);
    assert(q1 != NULL);
    /* now delete it, it should work. */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
    /* try to delete the queue again... this should fail. */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_failed);
    /* try to delete a NULL queue */
    status = SCPQueue_delete(NULL);
    assert(status == SCPStatus_failed);

    /* create 2 queues, and try to delete the first one. It should fail. */
    q1 = SCPQueue_create(1, 5);
    SCPContainer* q2 = SCPQueue_create(3, 4);
    assert(q1 != NULL);
    assert(q2 != NULL);
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_failed);
    /* now delete the second one, it should work. */
    status = SCPQueue_delete(q2);
    assert(status == SCPStatus_success);
    /* now delete the first one, it should work now. */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
}

static void SCPTests_testQueuePushPop(void)
{
    SCPContainer *q1;
    SCPStatus status;
    const uint16_t q1val1 = 0x1234, q1val2 = 0x5678, q1val3 = 0x9021, q1val4 = 0x1010;
    uint16_t q1out;
    
    /* create 2 queues*/
    q1 = SCPQueue_create(3, 2);
    assert (q1 != NULL);

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
    status = SCPQueue_push(NULL, (SCPAddr)&q1val1);
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
    status = SCPQueue_pop(NULL, (SCPAddr)&q1out);
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
    SCPContainer *q1;
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

    /* check that a NULL queue is empty */
    assert(SCPQueue_isEmpty(NULL) == SCPBool_true);

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
    SCPContainer *q1;
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

    /* check that a NULL queue is not full */
    assert(SCPQueue_isFull(NULL) == SCPBool_false);

    /* clean-up */
    status = SCPQueue_delete(q1);
    assert(status == SCPStatus_success);
    /* after deletion the queue should not be full */
    assert(SCPQueue_isFull(q1) == SCPBool_false);
}
