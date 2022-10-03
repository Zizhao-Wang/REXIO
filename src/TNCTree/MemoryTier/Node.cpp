/**
 * @date   2022.5.10 18:56
 * @author zz.wang
 * @details
 *          This file contains implementation of some block/node initialization methods!
 *
 *         IDENTIFICATION
 *   Experiment1/FrontEnd/MemoryAllocate/HashBlock.cpp
 */

#include "Node.h"
#include <cstdlib>
#include <climits>

/*
 * NIL node of skip list initialization.
 */

LocalHashNode * NILInitialize()
{
    auto * nil =(LocalHashNode *) malloc(sizeof LOCAL_HASH_SIZE);
    if(nil!= nullptr)
        nil->Hashkey=UINT64_MAX;
    return nil;
}


/*
 * Local node initialization.
 */

LocalHashNode * Initialization(SKey hashkey, uint32_t offset)
{
    auto * local =(LocalHashNode*) malloc(LOCAL_HASH_SIZE);
    if(local== nullptr)
        exit(0);
    local->Hashkey = hashkey;
    local->offset = offset;
    local->flag = 1;
    return local;
}


LocalHashNode * Initialization() 
{

    auto *local = (LocalHashNode *) malloc(LOCAL_HASH_SIZE);
    if (local == nullptr)
        exit(0);

    auto * NIL = NILInitialize();
    for(int i=0;i<MaxLevel;i++)
    {
        local->next[i]=NIL;
    }
    return local;
}



