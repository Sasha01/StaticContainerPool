/**
 * @file SCPCfg.h
 * @author Sorin Bugarin (sorin.bugarin@gmail.com)
 * @brief User configuration.
 * @version 0.0.1
 * @date 2023-01-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SCPCFG_H
#define SCPCFG_H

/***** USER DEFINED INCLUDES *****/
/*
 * If concurrency is used, define SCP_ENTER_CRITICAL() and SCP_EXIT_CRITICAL() to whatever you need.
 * This will probably need access to some header file... put it here.
*/


/***** DEFINES *******************/
/**
 * @brief The total size of the static pool used to create containers.
*/
#define SCP_TOTAL_BUFFER_SIZE 1000u

#endif /* SCPCFG_H */