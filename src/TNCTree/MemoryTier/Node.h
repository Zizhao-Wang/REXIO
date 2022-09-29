/**
 * @date 2022.4.27 15:01
 * @author zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 *         IDENTIFICATION
 *   Experiment1/FrontEnd/MemoryAllocate/HashBlock.h
 */

#ifndef EXPERIMENT1_HASHBLOCK_H
#define EXPERIMENT1_HASHBLOCK_H


#define LOCAL_HASH_SIZE  sizeof (LocalHashNode)
#define GLOBAL_HASH_SIZE sizeof (GlobalHashNode)
#define LOCAL_HEAD_SIZE  sizeof (LocalHeadNode)
#define MaxLevel 20


/*
 * Local hash table node.
 */
typedef struct LocalHashNode
{
    unsigned int Hashvalue;
    unsigned char flag;
    unsigned int offset;
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


/*  =================Node initialization module==================== */
    /*
     * Some methods of initialization.
     */
LocalHashNode * NILInitialize();

LocalHashNode * Initialization(unsigned int hashvalue, unsigned int offset);

LocalHashNode * Initialization();




#endif //EXPERIMENT1_HASHBLOCK_H
