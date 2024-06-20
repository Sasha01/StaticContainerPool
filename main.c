#include "SCPTests.h"
#include "SCPInternal.h"
#include <stdio.h>
int main(void)
{
    SCPTests_run();
    printf("Size of int: %llu\n", sizeof(int));
    printf("Size of int*: %llu\n", sizeof(int*));
    printf("Size of SCPQueue: %llu\n", sizeof(SCPQueue));
    printf("Size of SCPStack: %llu\n", sizeof(SCPStack));
    printf("Size of SCPContainer: %llu\n", sizeof(SCPContainer));
    return 0;
}