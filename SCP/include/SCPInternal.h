#include "SCPCfg.h"
#include "SCPTypes.h"

typedef struct {
    SCPContainer* nextFree;
    SCPUByte buffer[SCP_TOTAL_BUFFER_SIZE];
}SCPInternalType;

extern SCPInternalType scp;

#define CONTAINER_TYPE_MASK (0xFF)
#define CONTAINER_TYPE_POS  (1)

#define CONTAINER_FULL_MARKER (1)

#define IS_CONTAINER_FULL(ctnr)                     (((ctnr)->meta & CONTAINER_FULL_MARKER) ? SCPBool_true : SCPBool_false)

#define SET_CONTAINER_FULL(ctnr)                    ((ctnr)->meta |= CONTAINER_FULL_MARKER)

#define CLEAR_CONTAINER_FULL(ctnr)                  ((ctnr)->meta &= ~CONTAINER_FULL_MARKER)

#define START_OF_CONTAINER_DATA(queue)              (SCPAddr)((SCPAddr)(queue) + sizeof(SCPContainer))

#define IS_ADDR_IN_BUFFER_RANGE(addr)               ((((SCPAddr)(addr) >= scp.buffer) && \
                                                    ((SCPAddr)(addr) < &scp.buffer[SCP_TOTAL_BUFFER_SIZE])) ? \
                                                    SCPBool_true : SCPBool_false)

#define SET_CONTAINER_TYPE_MARKER(cntr, type)       ((cntr)->meta |= (((type) & CONTAINER_TYPE_MASK) << CONTAINER_TYPE_POS))

#define GET_CONTAINER_TYPE_MARKER(cntr)             (((cntr)->meta & CONTAINER_TYPE_MASK) >> CONTAINER_TYPE_POS)