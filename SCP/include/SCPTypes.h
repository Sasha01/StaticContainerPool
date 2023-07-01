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
#define SCP_INVALID (0xFFFFFFFFu)

typedef unsigned char SCPUByte;     /**< Type for one unsigned byte. */
typedef unsigned int SCPUWord;      /**< Type for one unsigned word. At least 4 bytes.*/
typedef unsigned char* SCPAddr;     /**< Type for addresses. */
typedef SCPUWord SCPContainerId; /**< Type used by clients to identify the working containter. */


/**
 * @brief Type used to represent boolean values.
 * 
 */
typedef enum {
    SCPBool_false,
    SCPBool_true
}SCPBool;

/**
 * @brief Type used to represent the status of an operation.
 * 
 */
typedef enum {
    SCPStatus_failed,
    SCPStatus_success
}SCPStatus;


#endif /* SCPTYPES_H */