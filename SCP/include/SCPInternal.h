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
 * @brief Enumeration that describes the type of the container.
 * 
 */
typedef enum {
    SCPContainerType_none = 0,
#if SCP_ENABLE_API_QUEUE
    SCPContainerType_queue,
#endif
#if SCP_ENABLE_API_STACK
    SCPContainerType_stack,
#endif
    SCPContainerType_free = 0xFF
}SCPContainerType;

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
    SCPUShort maxNoOfElem;   /**< Maximum number of elements in the queue.*/
    SCPUShort sizeOfElem;    /**< Size of an element in bytes.*/
    SCPUShort noOfElem;      /**< Number of elements currently in the container.*/
    SCPContainerType type;   /**< Represents the type of the container.*/
    SCPBool blocked;         /**< Flag used for synchronization. */
}SCPContainer;

typedef struct {
    SCPContainer* nextFree;
    SCPContainer* map[SCP_MAX_NO_OF_CONTAINERS];
    SCPUByte buffer[SCP_TOTAL_BUFFER_SIZE];
}SCPInternalType;

extern SCPInternalType scp;

#define START_OF_CONTAINER_DATA(ctnr)              (SCPAddr)((SCPAddr)(ctnr) + sizeof(SCPContainer))

#define IS_ADDR_IN_BUFFER_RANGE(addr)               ((((SCPAddr)(addr) >= scp.buffer) && \
                                                    ((SCPAddr)(addr) < &scp.buffer[SCP_TOTAL_BUFFER_SIZE])) ? \
                                                    SCPBool_true : SCPBool_false)

#define CONTAINER_DATA_SIZE(cntr)                   ((cntr)->maxNoOfElem * (cntr)->sizeOfElem)

#define END_OF_CONTAINER_DATA(cntr)                 (SCPAddr)((SCPAddr)(cntr) + sizeof(SCPContainer) + CONTAINER_DATA_SIZE(cntr))

#define IS_ADDR_IN_CONTAINER_RANGE(addr, cntr) (((SCPAddr)(addr) >= START_OF_CONTAINER_DATA(cntr)) && \
                                                ((SCPAddr)(addr) < END_OF_CONTAINER_DATA(cntr)))

#if SCP_ENABLE_PRINT_LOG
#include <stdio.h>
#define SCP_LOG(...) printf(__VA_ARGS__)
#else
#define SCP_LOG(...)
#endif 

typedef void (*SCPInitContainerFn)(SCPContainer* const container, const SCPUShort elem, const SCPUShort size);

SCPContainerId SCP_getNextFreeId(void);
SCPContainer* SCP_getContainer(const SCPContainerId contId);
void SCP_freeContainter(const SCPContainerId id);
SCPContainerId SCP_createContainer(const SCPUShort noOfElem, const SCPUShort sizeOfElem, const SCPInitContainerFn initFn);

#endif // SCPINTERNAL_H
