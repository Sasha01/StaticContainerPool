#include "SCPCfg.h"
#include "SCPTypes.h"
#include "SCPInternal.h"
#include <string.h>

SCPContainerId SCP_getNextFreeId(void)
{
    SCPContainerId id = 0;
    // search for the first free container
    for (id = 0; id < SCP_MAX_NO_OF_CONTAINERS; id++)
    {
        if (scp.map[id] == SCP_NULL)
        {
            // a free container was found, return
            break;
        }
    }
    // check the container was not found, return invalid
    if (id >= SCP_MAX_NO_OF_CONTAINERS)
    {
        id = SCP_INVALID;
    }

    return id;
}

SCPContainer* SCP_getContainer(const SCPContainerId contId)
{
    SCPContainer* cont = SCP_NULL;
    if ((contId < SCP_MAX_NO_OF_CONTAINERS) && (IS_ADDR_IN_BUFFER_RANGE(scp.map[contId])))
    {
        cont = scp.map[contId];
    }
    return cont;
}

void SCP_freeContainter(const SCPContainerId id)
{
    if (id < SCP_MAX_NO_OF_CONTAINERS)
    {
        SCP_ENTER_CRITICAL_SECTION();
        SCPContainer* const cntr = scp.map[id];

        RESET_CONTAINER_METADATA(cntr);
        memset(&cntr->c, 0x00, sizeof(cntr->c));

        /* check if it's the last queue created. */
        if (END_OF_CONTAINER_DATA(cntr) == (SCPAddr)scp.nextFree)
        {
            /*
            If it is the last queue that was created, move the nextFree pointer back
            and set the id in the map to NULL;
             */
            scp.nextFree = cntr;
            SET_CONTAINER_TYPE_MARKER(cntr, CONTAINER_TYPE_NONE);
            scp.map[id] = SCP_NULL;
        }
        else
        {
            /* 
            Trying to remove a container in the middle of the buffer.
            In this case, the id in the map will remain, just the type of 
            the container will be represented as empty, so the space can be 
            re-allocated later.
            */
            SET_CONTAINER_TYPE_MARKER(cntr, CONTAINER_TYPE_EMPTY);

        }
        SCP_EXIT_CRITICAL_SECTION();
    }
}