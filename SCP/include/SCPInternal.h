#ifndef SCPINTERNAL_H
#define SCPINTERNAL_H

#include "SCPCfg.h"
#include "SCPTypes.h"

#if SCP_ENABLE_API_QUEUE
/**
 * @brief Type that describes a queue. Only for internal use.
 * 
 */
typedef struct  {
    SCPAddr tail;           /**< Pointer to the tail of the queue.*/
    SCPAddr head;           /**< Pointer to the head of the queue.*/
}SCPQueue;
#endif  /* SCP_ENABLE_API_QUEUE */

#if SCP_ENABLE_API_STACK
/**
 * @brief Type that describes a stack. Only for internal use.
 * 
 */
typedef struct  {
    SCPAddr top;            /**< Pointer to the top of the stack.*/
}SCPStack;
#endif /* SCP_ENABLE_API_STACK */

/**
 * @brief Type that describes a generic container. Only for internal use.
 * 
 */
typedef struct {
    union {
        #if SCP_ENABLE_API_QUEUE
        SCPQueue q;         /**< Representation of the queue. */
        #endif  /* SCP_ENABLE_API_QUEUE */

        #if SCP_ENABLE_API_STACK
        SCPStack s;         /**< Representation of the stack. */
        #endif /* SCP_ENABLE_API_STACK */
    }c;                     /**< Representation of the container. */
    SCPUWord maxNoOfElem;   /**< Maximum number of elements in the queue.*/
    SCPUWord sizeOfElem;    /**< Size of an element in bytes.*/
    SCPUWord meta;          /**< Meta information. Reserved for future use.*/
}SCPContainer;

typedef struct {
    SCPContainer* nextFree;
    SCPContainer* map[SCP_MAX_NO_OF_CONTAINERS];
    SCPUByte buffer[SCP_TOTAL_BUFFER_SIZE];
}SCPInternalType;

extern SCPInternalType scp;

#define CONTAINER_TYPE_MASK (0xFF)
#define CONTAINER_TYPE_POS  (1)

#define CONTAINER_TYPE_FREE    (0xFF)
#define CONTAINER_TYPE_NONE     (0x00)

#define CONTAINER_FULL_MARKER (1)

#define IS_CONTAINER_FULL(ctnr)                     (((ctnr)->meta & CONTAINER_FULL_MARKER) ? SCPBool_true : SCPBool_false)

#define SET_CONTAINER_FULL(ctnr)                    ((ctnr)->meta |= CONTAINER_FULL_MARKER)

#define CLEAR_CONTAINER_FULL(ctnr)                  ((ctnr)->meta &= ~CONTAINER_FULL_MARKER)

#define START_OF_CONTAINER_DATA(ctnr)              (SCPAddr)((SCPAddr)(ctnr) + sizeof(SCPContainer))

#define IS_ADDR_IN_BUFFER_RANGE(addr)               ((((SCPAddr)(addr) >= scp.buffer) && \
                                                    ((SCPAddr)(addr) < &scp.buffer[SCP_TOTAL_BUFFER_SIZE])) ? \
                                                    SCPBool_true : SCPBool_false)

#define RESET_CONTAINER_METADATA(cntr)              ((cntr)->meta = 0)

#define REMOVE_CONTAINER_TYPE_MARKER(cntr)          ((cntr)->meta &= ~(CONTAINER_TYPE_MASK << CONTAINER_TYPE_POS))

#define SET_CONTAINER_TYPE_MARKER(cntr, type)       REMOVE_CONTAINER_TYPE_MARKER(cntr); \
                                                    ((cntr)->meta |= (((type) & CONTAINER_TYPE_MASK) << CONTAINER_TYPE_POS))

#define GET_CONTAINER_TYPE_MARKER(cntr)             (((cntr)->meta >> CONTAINER_TYPE_POS) & CONTAINER_TYPE_MASK)

#define IS_CONTAINER_FREE(cntr)                     (GET_CONTAINER_TYPE_MARKER(cntr) == CONTAINER_TYPE_FREE)

#define CONTAINER_DATA_SIZE(cntr)                   ((cntr)->maxNoOfElem * (cntr)->sizeOfElem)

#define END_OF_CONTAINER_DATA(cntr)                 (SCPAddr)((SCPAddr)(cntr) + sizeof(SCPContainer) + CONTAINER_DATA_SIZE(cntr))


SCPContainerId SCP_getFreeContainterId(const SCPUWord neededSize);
SCPContainerId SCP_getNextFreeId(void);
SCPContainer* SCP_getContainer(const SCPContainerId contId);
void SCP_freeContainter(const SCPContainerId id);

#endif // SCPINTERNAL_H
