#include "SCPCfg.h"
#include "SCPTypes.h"

typedef struct {
    SCPUByte buffer[SCP_TOTAL_BUFFER_SIZE];
    SCPAddr nextFree;
}SCPInternalType;

extern SCPInternalType scp;
