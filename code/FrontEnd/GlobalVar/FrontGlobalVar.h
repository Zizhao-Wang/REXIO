/**
 * @date 2022.4.30 13:43
 * @author zz.wang
 * @details
 *          If this is the program's first run, the program will create a new special hash table in memory.
 *          Otherwise, the process will read the hash table from the solid-state disk (SSD).
 *
 *         IDENTIFICATION
 *   Experiment1/FrontEnd/HashTable/HashTableCreate.h
 */

#ifndef EXPERIMENT1_FRONTGLOBALVAR_H
#define EXPERIMENT1_FRONTGLOBALVAR_H


#include "../MemoryAllocate/HashBlock.h"
#include <vector>


#define LOCAL_TABLE_SIZE 100000


extern int Globaldepth;


/*
 *  Hash table
 */
extern std::vector<GlobalHashNode *> global;


extern float pro;


#define DEBUG true
#endif //EXPERIMENT1_FRONTGLOBALVAR_H
