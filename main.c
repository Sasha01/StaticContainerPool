#include "SCPTests.h"
#include "SCPInternal.h"
#include <stdio.h>
int main(void)
{
    SCPTests_run();
    printf("Size of int: %ld\n", sizeof(int));
    printf("Size of int*: %ld\n", sizeof(int*));
    printf("Size of SCPQueue: %ld\n", sizeof(SCPQueue));
    printf("Size of SCPStack: %ld\n", sizeof(SCPStack));
    printf("Size of SCPContainer: %ld\n", sizeof(SCPContainer));
    return 0;
}