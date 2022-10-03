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


/*
 * Global hash table node.
 */

typedef struct GlobalHashNode
{
    unsigned int bit;
    LocalHeadNode * local;
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
LocalHashNode * NILInitialize();

LocalHashNode * Initialization(SKey hashkey, uint32_t offset);

LocalHashNode * Initialization();




#endif //EXPERIMENT1_HASHBLOCK_H
