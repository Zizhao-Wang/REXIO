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

#include "../../Auxizilary/GlobalVariable.h"

#define LOCAL_HASH_SIZE  sizeof (LocalHashNode)
#define GLOBAL_HASH_SIZE sizeof (GlobalHashNode)
#define LOCAL_HEAD_SIZE  sizeof (LocalHeadNode)
#define MaxLevel 20

const int MAX_LEVEL1 = 32;
const int P_FACTOR1 = RAND_MAX >> 2;

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


typedef struct TSkiplistNode 
{
    uint64_t key;
    uint8_t flag;
    uint32_t offset;
    int maxLevel;
    struct TSkiplistNode **forward;
} TSkiplistNode;


typedef struct 
{
    TSkiplistNode *head;
    int number;
    int level;
    int depth;
} TNCSkiplist;

/*
 * Global hash table node.
 */

typedef struct GlobalHashNode
{
    unsigned int bit;
    TNCSkiplist * local;
}GlobalHashNode;

/**
 *  Definition of key-value pair
 **/
typedef struct TNCEntry
{
    SKey key;
    SValue val;
}TNCEntry;






/*  =================Node initialization module==================== */
    /*
     * Some methods of initialization.
     */
TSkiplistNode * TskiplistNodeCreat(uint64_t key,uint32_t offset, int maxLevel);

TNCSkiplist *  TskiplistCreate();

LocalHashNode * NILInitialize();

LocalHashNode * Initialization(SKey hashkey, uint32_t offset);

LocalHashNode * Initialization();




#endif //EXPERIMENT1_HASHBLOCK_H
