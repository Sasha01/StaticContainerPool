/**
 * @file SCPQueue.h
 * @author Sorin Bugarin (sorin.bugarin@gmail.com)
 * @brief API for handling queue operations.
 * @version 0.0.1
 * @date 2023-01-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SCPQUEUE_H
#define SCPQUEUE_H

#include "SCPCfg.h"
#include "SCPTypes.h"

/**
 * @brief Creates a queue from the static pool.
 * 
 * @param noOfElem - Maximum number of elements that the queue must accomodate.
 * @param sizeOfElem - Size of one element in bytes.
 * @return SCPContainerId - SCP_INVALID if failed
 */
SCPContainerId SCPQueue_create(const SCPUShort noOfElem, const SCPUShort sizeOfElem);

/**
 * @brief Deletes the queue.
 * @details Only the last created container can be deleted for now. Later on the plan is to add the functionality to delete any container.
 * @param id Queue to be deleted.
 * @return SCPStatus - success/failed
 */
SCPStatus SCPQueue_delete(const SCPContainerId id);

/**
 * @brief Pushes one element in the queue.
 * 
 * @param id - reference to the queue.
 * @param data - element to be pushed.
 * @return SCPStatus - success/failed
 */
SCPStatus SCPQueue_push(const SCPContainerId id, SCPAddr data);

/**
 * @brief Extracts one element from the queue.
 * 
 * @param id - reference to the queue.
 * @param data - pointer to where the data from the extracted element will be copied.
 * @return SCPStatus - success/failed
 */
SCPStatus SCPQueue_pop(const SCPContainerId id, SCPAddr data);

/**
 * @brief Check if the queue is empty.
 * 
 * @param id - reference to the queue.
 * @return SCPBool - true = the queue is empty, false = the queue is not empty.
 */
SCPBool SCPQueue_isEmpty(const SCPContainerId id);

/**
 * @brief Checks if the queue is full.
 * 
 * @param id - reference to the queue.
 * @return SCPBool - true = the queue is full, false = the queue is not full.
 */
SCPBool SCPQueue_isFull(const SCPContainerId id);

/**
 * @brief Gets the number of elements that are currently in the queue.
 * 
 * @param id - reference to the queue.
 * @return SCPUWord - number of elements currently in the queue. If the queue is not valid, returns 0.
 */
SCPUWord SCPQueue_getCount(const SCPContainerId id);
#endif // SCPQUEUE_H
