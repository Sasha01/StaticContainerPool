#include "SCP.h"
#include "SCPInternal.h"

/**
 * @brief The internal buffer. 
 * @details This should be the only global variable used by SCP. Every access to this should be guarded for concurrency.
 * 
 */
SCPInternalType scp;
