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

#ifdef NOT_SEPARATE_KV
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint32_t offset, int maxLevel)
#elif defined(NOT_SEPARATE_KV_variable)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint32_t offset, int maxLevel)
#elif defined(SEPARATE_KV_FIXED_LOG)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint32_t offset,uint32_t block1, int maxLevel)
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint32_t offset,uint32_t block1, int maxLevel)
#endif
{
    TSkiplistNode *obj = (TSkiplistNode *)malloc(sizeof(TSkiplistNode));
    memcpy(obj->key, key, KEY_SIZE);
    obj->offset = offset;
    obj->flag = 1;

#ifdef SEPARATE_KV_FIXED_LOG
    obj->block = block1;
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    obj->block = block1;
#endif

    obj->maxLevel = maxLevel;
    obj->forward = (TSkiplistNode **)malloc(sizeof(TSkiplistNode *) * maxLevel);
    for (int i = 0; i < maxLevel; i++) 
    {
        obj->forward[i] = NULL;
    }
    return obj;
}

TNCSkiplist * TskiplistCreate()
{
    TNCSkiplist *obj = (TNCSkiplist *)malloc(sizeof(TNCSkiplist));
    char key[KEY_SIZE];
    memset(key, 0, KEY_SIZE); 

#ifdef NOT_SEPARATE_KV
    obj->head = TskiplistNodeCreat(key,0, MAX_LEVEL1);
#elif defined(NOT_SEPARATE_KV_variable)
    obj->head = TskiplistNodeCreat(key,0, MAX_LEVEL1);
#elif defined(SEPARATE_KV_FIXED_LOG)
    obj->head = TskiplistNodeCreat(key,0,0, MAX_LEVEL1);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    obj->head = TskiplistNodeCreat(key,0,0, MAX_LEVEL1);
#endif

    obj->level = 0;
    obj->number = 0;
    srand(time(NULL));
    return obj;

}





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



