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


typedef unsigned char SCPUByte;     /**< Type for one unsigned byte. */
typedef unsigned int SCPUWord;      /**< Type for one unsigned word. At least 4 bytes.*/
typedef unsigned char* SCPAddr;     /**< Type for addresses. */

/**
 * @brief Type that describes a queue. Only for internal use.
 * 
 */
typedef struct  {
    SCPUWord maxNoOfElem;   /**< Maximum number of elements in the queue.*/
    SCPUWord sizeOfElem;    /**< Size of an element in bytes.*/
    SCPUWord meta;          /**< Meta information. Reserved for future use.*/
    SCPAddr tail;           /**< Pointer to the tail of the queue.*/
    SCPAddr head;           /**< Pointer to the head of the queue.*/
}SCPQueue;



#endif /* SCPTYPES_H */