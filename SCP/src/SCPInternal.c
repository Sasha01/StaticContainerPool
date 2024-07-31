#include "SCPCfg.h"
#include "SCPTypes.h"
#include "SCPInternal.h"
#include <string.h>
#include <assert.h>

static void mergeNextFreeContainers(SCPContainer* const cntr)
{
    /*
    Given a freed container, go through the next containers and if they are free merge them together.
    Do this until the first non-free container is found or we reach the end of the containers.
    */
    assert(cntr->type == SCPContainerType_free);
    assert(cntr->sizeOfElem == 1);
    SCPContainer* nextCntr = (SCPContainer*)END_OF_CONTAINER_DATA(cntr);
    while ((nextCntr < scp.nextFree) && (nextCntr->type == SCPContainerType_free))
    {
        SCPContainerId it;
        cntr->maxNoOfElem += (nextCntr->maxNoOfElem + sizeof(SCPContainer));
        /*
        Delete the id from the map.
        We no longer need it because we already have the id of the first free container.
        This next container will no longer exist.
        */
        for (it = 0; it < SCP_MAX_NO_OF_CONTAINERS; it++)
        {
            if (scp.map[it] == nextCntr)
            {
                scp.map[it] = SCP_NULL;
            }
        }
        SCP_LOG("Merged container of %d bytes created at address %p with container of %d bytes at address %p\n", 
            nextCntr->maxNoOfElem, 
            (void*)((SCPAddr)nextCntr - (SCPAddr)scp.buffer),
            cntr->maxNoOfElem - nextCntr->maxNoOfElem,
            (void*)((SCPAddr)cntr - (SCPAddr)scp.buffer)
            );
        nextCntr = (SCPContainer*)END_OF_CONTAINER_DATA(nextCntr);

    }  
}

static SCPContainerId getFreeContainterId(const SCPUWord neededSize)
{
    /*
    Search for an empty container that can fit the data size required.
    Return the ID of the container of minimum size >= neededSize.
    */
    SCPContainerId id = SCP_INVALID;
    SCPContainerId it;  /* iterator */

    for (it = 0; it < SCP_MAX_NO_OF_CONTAINERS; it++)
    {
        if ((scp.map[it] != SCP_NULL) && (scp.map[it]->type == SCPContainerType_free))
        {
            if (CONTAINER_DATA_SIZE(scp.map[it]) == neededSize)
            {
                /* just return the id */
                id = it;
                break;
            }

            /* If the empty container is not the exact same size as the needed container, we need to be able to 
            create one more container in the remaining space. So the size of the currently empty container needs
            to fit the new container and another one with at least 1 byte of data. */
            else if (CONTAINER_DATA_SIZE(scp.map[it]) >= (neededSize + sizeof(SCPContainer) + 1))
            {
                /* First of all, we need to add the new id to the map */
                SCPContainerId it2;
                for (it2 = 0; it2 < SCP_MAX_NO_OF_CONTAINERS; it2++)
                {
                    if (scp.map[it2] == SCP_NULL)
                    {
                        /* The new container will be created after this function returns, but the remaining 
                        space needs to be set up with a new container now. */

                        SCPContainer* newFreeContainer = (SCPContainer*)((SCPAddr)scp.map[it] + sizeof(SCPContainer) + neededSize);
                        newFreeContainer->type = SCPContainerType_free;
                        newFreeContainer->maxNoOfElem = CONTAINER_DATA_SIZE(scp.map[it]) - sizeof(SCPContainer) - neededSize;
                        newFreeContainer->sizeOfElem = 1;
                        scp.map[it2] = newFreeContainer;
                        SCP_LOG("New free container of %d bytes created at address %p\n", newFreeContainer->maxNoOfElem, (void*)((SCPAddr)newFreeContainer - (SCPAddr)scp.buffer));
                        /* TODO: if the next container if a free container, join them. */
                        mergeNextFreeContainers(newFreeContainer);
                        /* The free container was created successfully, so now we can return the id back to the caller
                        so it can create the needed container. */
                        id = it;
                        break;
                    }
                }
                /* check if we can break out of the loop */
                if (id == it)
                {
                    break;
                }


            }
            else
            {
                /* do nothing, we can't create the container here. */
            }
        }
        else
        {
            /* do nothing */
        }
    }

    return id;
}

SCPContainerId SCP_getNextFreeId(void)
{
    SCPContainerId id = 0;
    // search for the first free id in the map
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

        memset(&cntr->c, 0x00, sizeof(cntr->c));

        /* check if it's the last queue created. */
        if (END_OF_CONTAINER_DATA(cntr) == (SCPAddr)scp.nextFree)
        {
            /*
            If it is the last queue that was created, move the nextFree pointer back
            and set the id in the map to NULL;
             */
            scp.nextFree = cntr;
            scp.map[id] = SCP_NULL;
            SCP_LOG("Removed container of %d bytes created at address %p\n", cntr->maxNoOfElem * cntr->sizeOfElem, (void*)((SCPAddr)cntr - (SCPAddr)scp.buffer));
        }
        else
        {
            /* 
            Trying to remove a container in the middle of the buffer.
            In this case, the id in the map will remain, just the type of 
            the container will be represented as empty, so the space can be 
            re-allocated later.
            */
            cntr->type = SCPContainerType_free;
            /*
            Update the internal data so it will be represented with sizeOfElem = 1
            and maxNoOfElem = numberOfBytes
            */
            cntr->maxNoOfElem *= cntr->sizeOfElem;
            cntr->sizeOfElem = 1;
            cntr->noOfElem = 0;
            cntr->blocked = SCPBool_false;
            SCP_LOG("Freed container of %d bytes created at address %p\n", cntr->maxNoOfElem * cntr->sizeOfElem, (void*)((SCPAddr)cntr - (SCPAddr)scp.buffer));
            mergeNextFreeContainers(cntr);
        }
        
        SCP_EXIT_CRITICAL_SECTION();
    }
}

SCPContainerId SCP_createContainer(const SCPUShort noOfElem, const SCPUShort sizeOfElem, const SCPInitContainerFn initFn)
{
    SCPContainerId id = SCP_INVALID;
    if ((noOfElem > 0) && (sizeOfElem > 0))
    {
        SCP_ENTER_CRITICAL_SECTION();

        /* first, check if there is an already created container that is freed and can fit the data */

        id = getFreeContainterId(noOfElem * sizeOfElem);
        if (id != SCP_INVALID)
        {
            SCPContainer* const q = SCP_getContainer(id);
            initFn(q, noOfElem, sizeOfElem);
            SCP_LOG("Container of %d bytes created at address %p\n", noOfElem * sizeOfElem, (void*)((SCPAddr)q - (SCPAddr)scp.buffer));
            // this will lead to fragmented (lost) buffer space if the new container size is less than the previous container size.
            /*
            TODO: need to find a way to deal with this...
            Idea: In getFreeContainterId() only return if the found empty container size is equal to the new container size, OR
            if there is enough space to create a new free container with at least 1 byte of data.
            This way we could implement a defragment function that will run at low CPU load and bring all the free containers to the
            end of the buffer doing safe memmove. This will potentially take a long time, and it needs to be done in a critical section.
            Idea 2: each call to the defragment function will move ONE empty container. If one container was moved, it will return IN_PROGRESS,
            if no empty conainer was found return SUCCESS.
            */

        }
        else
        {
            /* no free container found, check if there is enough space in the buffer to create a new one. */
            /* check the free space in the buffer */
            SCPUWord neededSpace = sizeof(SCPContainer) + (noOfElem * sizeOfElem);
            SCPUWord freeSpace = SCP_TOTAL_BUFFER_SIZE - ((SCPAddr)scp.nextFree - scp.buffer);
            if (freeSpace >= neededSpace)
            {
                /* there is enough space for the container, create it */
                id = SCP_getNextFreeId();
                if (id != SCP_INVALID)
                {
                    SCPContainer* const newContainer = scp.nextFree; 
                    initFn(newContainer, noOfElem, sizeOfElem);
                    scp.nextFree = (SCPContainer*)END_OF_CONTAINER_DATA(scp.nextFree);
                    scp.map[id] = newContainer;  
                    SCP_LOG("Container of %d bytes created at address %p\n", noOfElem * sizeOfElem, (void*)((SCPAddr)newContainer - (SCPAddr)scp.buffer));
                }
            }
        }
        SCP_EXIT_CRITICAL_SECTION();
    }
    
    return id;
}