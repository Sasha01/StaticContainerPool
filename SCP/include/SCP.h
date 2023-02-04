/**
 * @file SCP.h
 * @author Sorin Bugarin (sorin.bugarin@gmail.com)
 * @brief Main include file that provides access to all functionality.
 * @version 0.0.1
 * @date 2023-01-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef SCP_H
#define SCP_H

#include "SCPCfg.h"
#include "SCPTypes.h"

void SCP_init(void);

#if SCP_ENABLE_API_QUEUE
#include "SCPQueue.h"
#endif /* SCP_ENABLE_API_QUEUE */


#endif /* SCP_H*/