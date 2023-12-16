/**
 * @date    27/9/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/MemoryTier/Node.h
 */

#ifndef EXPERIMENT1_INCLUDE_NODE_H
#define EXPERIMENT1_INCLUDE_NODE_H

#include <bitset>
#include "pre_definition.h"
#include "micros.h"

#define LOCAL_HASH_SIZE  sizeof (LocalHashNode)
#define GLOBAL_HASH_SIZE sizeof (GlobalHashNode)
#define LOCAL_HEAD_SIZE  sizeof (LocalHeadNode)
#define MaxLevel 32
#define KEY_SIZE 16


const int MAX_LEVEL1 = 32; 
const double P_FACTOR1 = 0.5;
const int max_bucket_size = 1024;


/*
 * Local hash table node.
 */
typedef struct LocalHashNode
{
    char key[KEY_SIZE];
    uint64_t offset;
    uint8_t flag;
#ifndef NOT_SEPARATE_KV
    uint64_t block;   // 40-bit bitset for block
#endif
    int maxLevel;
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
    uint32_t ky;
    uint8_t flag;
    uint64_t offset;  // 40-bit bitset for offset
#ifndef NOT_SEPARATE_KV
    uint64_t block;   // 40-bit bitset for block
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
#ifdef FastSkiplist
    typedef struct GlobalHashNode {
        unsigned int bit;
        LocalHeadNode *local;
    } GlobalHashNode;
#else
    typedef struct GlobalHashNode {
        unsigned int bit;
        TNCSkiplist *local;
    } GlobalHashNode;
#endif




/**
 *  Definition of key-value pair
 **/





/*  =================Node initialization module==================== */
    /*
     * Some methods of initialization.
     */
#ifdef NOT_SEPARATE_KV
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint64_t offset, int maxLevel);
#elif defined(NOT_SEPARATE_KV_variable)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint64_t offset, int maxLevel);
#elif defined(SEPARATE_KV_FIXED_LOG)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint64_t offset,uint64_t block1, int maxLevel);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    TSkiplistNode *TskiplistNodeCreat(const char* key,uint64_t offset,uint64_t block1, int maxLevel);
#endif


TNCSkiplist *  TskiplistCreate();





LocalHashNode * NILInitialize();

#ifdef NOT_SEPARATE_KV
    LocalHashNode *Initialization(const char* hashkey,uint64_t offset, int maxLevel);
#elif defined(NOT_SEPARATE_KV_variable)
    LocalHashNode *Initialization(const char* hashkey,uint64_t offset, int maxLevel);
#elif defined(SEPARATE_KV_FIXED_LOG)
    LocalHashNode *Initialization(const char* hashkey,uint64_t offset,uint64_t block1, int maxLevel);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    LocalHashNode *Initialization(const char* hashkey,uint64_t offset,uint64_t block1, int maxLevel);
#endif

LocalHashNode * Initialization();




#endif 
