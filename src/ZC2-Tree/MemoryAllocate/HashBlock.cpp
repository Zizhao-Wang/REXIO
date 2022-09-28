/**
 * @date   2022.5.10 18:56
 * @author zz.wang
 * @details
 *          This file contains implementation of some block/node initialization methods!
 *
 *         IDENTIFICATION
 *   Experiment1/FrontEnd/MemoryAllocate/HashBlock.cpp
 */

#include "HashBlock.h"
#include <cstdlib>
#include <climits>

/*
 * NIL node of skip list initialization.
 */

LocalHashNode * NILInitialize()
{
    auto * nil =(LocalHashNode *) malloc(sizeof LOCAL_HASH_SIZE);
    if(nil!= nullptr)
        nil->Hashvalue=UINT_MAX;
    return nil;
}


/*
 * Local node initialization.
 */

LocalHashNode * Initialization(unsigned int hashvalue, unsigned int offset)
{
    auto * local =(LocalHashNode*) malloc(LOCAL_HASH_SIZE);
    if(local== nullptr)
        exit(0);
    local->Hashvalue = hashvalue;
    local->offset = offset;
    local->flag = 1;
    return local;
}


LocalHashNode * Initialization() {

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



