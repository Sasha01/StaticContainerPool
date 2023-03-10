/**
 * @file SCPTypes.h
 * @author Sorin Bugarin (sorin.bugarin@gmail.com)
 * @brief File containing all types that are needed for SCP.
 * @version 0.0.1
 * @date 2023-01-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SCPTYPES_H
#define SCPTYPES_H

#include "SCPCfg.h"

#define SCP_NULL 0

typedef unsigned char SCPUByte;     /**< Type for one unsigned byte. */
typedef unsigned int SCPUWord;      /**< Type for one unsigned word. At least 4 bytes.*/
typedef unsigned char* SCPAddr;     /**< Type for addresses. */

/**
 * @brief Type used to represent boolean values.
 * 
 */
typedef enum {
    SCPBool_false,
    SCPBool_true
}SCPBool;

#if SCP_ENABLE_API_QUEUE
/**
 * @brief Type that describes a queue. Only for internal use.
 * 
 */
typedef struct  {
    SCPUWord maxNoOfElem;   /**< Maximum number of elements in the queue.*/
    SCPUWord sizeOfElem;    /**< Size of an element in bytes.*/
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
    SCPUWord maxNoOfElem;   /**< Maximum number of elements in the stack.*/
    SCPUWord sizeOfElem;    /**< Size of an element in bytes.*/
    SCPAddr top;           /**< Pointer to the top of the stack.*/
}SCPStack;
#endif /* SCP_ENABLE_API_STACK */

/**
 * @brief Type that describes a generic container. Only for internal use.
 * 
 */
typedef struct {
    SCPUWord meta;          /**< Meta information. Reserved for future use.*/
    union {
        #if SCP_ENABLE_API_QUEUE
        SCPQueue q;         /**< Representation of the queue. */
        #endif  /* SCP_ENABLE_API_QUEUE */

        #if SCP_ENABLE_API_STACK
        SCPStack s;         /**< Representation of the stack. */
        #endif /* SCP_ENABLE_API_STACK */
    }c;                     /**< Representation of the container. */
}SCPContainer;

/**
 * @brief Type used to represent the status of an operation.
 * 
 */
typedef enum {
    SCPStatus_failed,
    SCPStatus_success
}SCPStatus;


#endif /* SCPTYPES_H */