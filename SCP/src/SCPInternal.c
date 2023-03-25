#include "SCPCfg.h"
#include "SCPTypes.h"
#include "SCPInternal.h"

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
        SET_CONTAINER_TYPE_MARKER(scp.map[id], CONTAINER_TYPE_NONE);
        scp.map[id] = SCP_NULL;
    }
}