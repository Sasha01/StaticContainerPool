
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "SCP.h"

static void SCPTests_testQueue(void);

void SCPTests_run(void)
{
    #if SCP_ENABLE_API_QUEUE 
    SCPTests_testQueue();
    #endif  /* SCP_ENABLE_API_QUEUE */
    printf("Running SCP tests! Success!\n");
}

static void SCPTests_testQueue(void)
{
        /* Test queue creation. */
    SCPContainer* q1 = SCPQueue_create(3,2);
    assert(q1 != NULL);
    assert(SCPQueue_isEmpty(q1) == 1);
    SCPContainer* q2 = SCPQueue_create(100,9);
    assert(q2 != NULL);
    /* Assuming the size of the buffer is 1000B, this next queue creation is expected to fail due to lack of space. */
    SCPContainer* q3 = SCPQueue_create(5,10);
    assert(q3 == NULL);

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
    printf("Testing queues. Succes!!!\n");
}