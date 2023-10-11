/**
 * @date    27/9/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/MemoryTier/Node.h
 */

#ifndef EXPERIMENT1_HASHBLOCK_H
#define EXPERIMENT1_HASHBLOCK_H

#include "pre_definition.h"
#include "micros.h"

#define LOCAL_HASH_SIZE  sizeof (LocalHashNode)
#define GLOBAL_HASH_SIZE sizeof (GlobalHashNode)
#define LOCAL_HEAD_SIZE  sizeof (LocalHeadNode)
#define MaxLevel 20

const int MAX_LEVEL1 = 32;
const int P_FACTOR1 = RAND_MAX >> 2;
const int max_bucket_size = 32768;

/*
 * Local hash table node.
 */
typedef struct LocalHashNode
{
    uint64_t Hashkey;
    uint32_t offset;
    uint8_t flag;
    LocalHashNode * next[MaxLevel];
}LocalHashNode;


/*
 * Local hash table head.
 */
typedef struct LocalHeadNode
{
    int    CurrentLevel;
    int    Nodenumber;
    int    depth;
    LocalHashNode * HashNode;
}LocalHeadNode;



/** 
 *  @author: zz.wang
 *  @date:   7/5/2023
 *  ============  another Skiplist implementation =========
 **/ 

typedef struct TSkiplistNode 
{
    char key[KEY_SIZE];
    uint8_t flag;
    uint32_t offset;
#ifndef NOT_SEPARATE_KV
    uint32_t block;
#endif
    int maxLevel;
    struct TSkiplistNode **forward;
} TSkiplistNode;


typedef struct 
{
    TSkiplistNode *head;
    int number;
    int level;
    int depth;
}TNCSkiplist;

/*
 * Global hash table node.
 */

typedef struct GlobalHashNode
{
    unsigned int bit;
    TNCSkiplist * local;
}GlobalHashNode;

// typedef struct GlobalHashNode
// {
//     unsigned int bit;
//     LocalHeadNode * local;
// }GlobalHashNode;



/**
 *  Definition of key-value pair
 **/

typedef struct key_value_entry
{
    char key[KEY_SIZE];
    char val[VAL_SIZE];
}key_value_entry;





/*  =================Node initialization module==================== */
    /*
     * Some methods of initialization.
     */
#ifdef NOT_SEPARATE_KV
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint32_t offset, int maxLevel);
#elif defined(NOT_SEPARATE_KV_variable)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint32_t offset, int maxLevel);
#elif defined(SEPARATE_KV_FIXED_LOG)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint32_t offset,uint32_t block1, int maxLevel);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint32_t offset,uint32_t block1, int maxLevel);
#endif


TNCSkiplist *  TskiplistCreate();



LocalHashNode * NILInitialize();

LocalHashNode * Initialization(key_type hashkey, uint32_t offset);

LocalHashNode * Initialization();




#endif //EXPERIMENT1_HASHBLOCK_H
