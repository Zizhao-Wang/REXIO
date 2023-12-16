/**
 * @date   2022.5.10 18:56
 * @author zz.wang
 * @details
 *          This file contains implementation of some block/node initialization methods!
 *
 *         IDENTIFICATION
 *   Experiment1/FrontEnd/MemoryAllocate/HashBlock.cpp
 */

#include "include/node.h"
#include <cstdlib>
#include <climits>
#include <cstdint>


uint64_t total_write_bytes = 0; // write I/Os



#ifdef NOT_SEPARATE_KV
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint64_t offset, int maxLevel)
#elif defined(NOT_SEPARATE_KV_variable)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint64_t offset, int maxLevel)
#elif defined(SEPARATE_KV_FIXED_LOG)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint64_t offset,uint64_t block1, int maxLevel)
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint64_t offset,uint64_t block1, int maxLevel)
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
    obj->forward = (TSkiplistNode **)malloc(sizeof(TSkiplistNode *) * (maxLevel+1));
    if(obj->forward == NULL){
        fprintf(stderr, "malloc failed when creating the node in skiplist.\n");
        exit(1);
    }
    for (int i = 0; i <= maxLevel; i++) 
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

    auto * nil =TskiplistNodeCreat(key,0,0, MAX_LEVEL1);;
    memset(nil->key, 0xFF, KEY_SIZE); 
    nil->ky = UINT32_MAX;
    for(int i=0;i<=MAX_LEVEL1;i++)
    {
        obj->head->forward[i]=nil;
    }
    obj->head->ky = 0;

    for(int i=0;i<=MAX_LEVEL1;i++)
    {
        nil->forward[i]=NULL;
    }

    obj->level = 0;
    obj->number = 0;
    return obj;
}





/*
 * NIL node of skip list initialization.
 */

LocalHashNode * NILInitialize()
{
    auto * nil =(LocalHashNode *) malloc(sizeof LOCAL_HASH_SIZE);

    char key[KEY_SIZE];
    memset(key, 0xFF, KEY_SIZE); 

    if(nil!= nullptr){
        memcpy(nil->key, key, KEY_SIZE);
    }
        
    return nil;
}


/*
 * Local node initialization.
 */

#ifdef NOT_SEPARATE_KV
    LocalHashNode *Initialization(const char* hashkey,uint64_t offset, int maxLevel)
#elif defined(NOT_SEPARATE_KV_variable)
    LocalHashNode *Initialization(const char* hashkey,uint64_t offset, int maxLevel)
#elif defined(SEPARATE_KV_FIXED_LOG)
    LocalHashNode *Initialization(const char* hashkey,uint64_t offset,uint64_t block1, int maxLevel)
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    LocalHashNode *Initialization(const char* hashkey,uint64_t offset,uint64_t block1, int maxLevel)
#endif
{
    auto * local =(LocalHashNode*) malloc(LOCAL_HASH_SIZE);
    if(local== nullptr)
    {
        fprintf(stdout,"can not request memory!\n");
        fflush(stdout);
        exit(0);
    }
    memcpy(local->key, hashkey, KEY_SIZE);
    local->offset = offset;
    local->flag = 1;
#ifdef SEPARATE_KV_FIXED_LOG
    local->block = block1;
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    local->block = block1;
#endif
    return local;
}


LocalHashNode * Initialization() 
{

    auto *local = (LocalHashNode *) malloc(LOCAL_HASH_SIZE);
    if (local == nullptr)
    {
        fprintf(stdout,"request memory errror!\n");
        exit(0);
    }
    memset(local->key, 0, KEY_SIZE);
    auto * NIL = NILInitialize();
    for(int i=0;i<MaxLevel;i++)
    {
        local->next[i]=NIL;
    }
    return local;
}



