
#include <string.h>
#include "SCP.h"
#include "SCPInternal.h"

/**
 * @brief The internal buffer. 
 * @details This should be the only global variable used by SCP. Every access to this should be guarded for concurrency.
 * 
 */
SCPInternalType scp;

void SCP_init(void)
{
    memset(scp.buffer, 0x00, sizeof(scp.buffer));
    memset(scp.map, 0x00, sizeof(scp.map));
    scp.nextFree = (SCPContainer*)scp.buffer;
}