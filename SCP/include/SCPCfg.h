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

/**
 * @brief Macro for entering a critical section. This should prevent the current task from being preempted while executing in a critical section. To be defined by user.
 * 
 */
#define SCP_ENTER_CRITICAL_SECTION()

/**
 * @brief Macro for exiting from a critical section. To be defined by user.
 * 
 */
#define SCP_EXIT_CRITICAL_SECTION()

/**
 * @brief Macro to enable or disable queue handling. If queues are not needed, set this to 0.
 * 
 */
#define SCP_ENABLE_API_QUEUE 1

/**
 * @brief Macro to enable or disable stack handling. If stacks are not needed, set this to 0.
 * 
 */
#define SCP_ENABLE_API_STACK 1

#endif /* SCPCFG_H */
