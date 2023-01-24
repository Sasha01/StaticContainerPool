#include "SCPCfg.h"
#include "SCPTypes.h"

typedef struct {
    SCPContainer* nextFree;
    SCPUByte buffer[SCP_TOTAL_BUFFER_SIZE];
}SCPInternalType;

extern SCPInternalType scp;
