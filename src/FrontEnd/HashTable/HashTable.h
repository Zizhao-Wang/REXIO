/**
 * @date 2022.4.27 10:40
 * @author zz.wang
 * @details
 *          If this is the program's first run, the program will create a new special hash table in memory.
 *          Otherwise, the process will read the hash table from the solid-state disk (SSD).
 *
 *         IDENTIFICATION
 *   Experiment1/FrontEnd/HashTable/HashTableCreate.h
 */
#ifndef EXPERIMENT1_HASHTABLE_H
#define EXPERIMENT1_HASHTABLE_H

#include <cstdint> 
#include "../MemoryAllocate/HashBlock.h"


/* =================Node initialization module==================== */
bool LocalGeneration(GlobalHashNode * global);

bool DoubleHashtable();


/* =================Node Searching module==================== */
   /*
    *  Searching
    */
LocalHashNode* SearchNode(LocalHeadNode* Head,unsigned int hashvalue);


/*  =================Node Insertion module====================  */
    /*
     * Insertation
     */
int RandomLevel();

LocalHeadNode * NodeSplit(LocalHeadNode * head);

bool InsertNode(uint64_t hashvalue);

int Insert(uint64_t hashvalue);


/*  =================Node deletion module====================  */
bool DeleteNode(LocalHeadNode * Head, unsigned int hashvalue);


/*  =================Hash table initialization module====================  */
int ExtendHashTableInitialize();


#endif //EXPERIMENT1_HASHTABLE_H
