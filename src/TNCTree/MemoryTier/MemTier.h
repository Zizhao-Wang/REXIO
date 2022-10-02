/**
 * @date   27/4/2022 
 * @author zz.wang
 * @details
 *         If this is the program's first run, the program will create a new special hash table in memory.
 *         Otherwise, the process will read the hash table from the solid-state disk (SSD).
 *
 * IDENTIFICATION:
 *         src/FrontEnd/HashTable/HashTable.h
 */
#ifndef EXPERIMENT1_HASHTABLE_H
#define EXPERIMENT1_HASHTABLE_H

#include <stdint.h>
#include "../../Auxizilary/GlobalVariable.h" 
#include "Node.h"

/**
 *  ================= Node initialization module ==================== 
 **/
bool LocalGeneration(GlobalHashNode * global);

bool DoubleHashtable();


/**
 *  =================  Search module  ==================== 
 **/

LocalHashNode* SearchNode(LocalHeadNode* Head,unsigned int hashvalue);


/**  
 * ================= Insertion module ====================  
 **/

int RandomLevel();

LocalHeadNode * NodeSplit(LocalHeadNode * head);

bool InsertNode(uint64_t hashvalue);

int Insert(SKey hashvalue, SValue);


/**
 *  ================= deletion module====================  
 **/
bool DeleteNode(LocalHeadNode * Head, unsigned int hashvalue);


/**  
 * ================= Memory-tier initialization module ====================  
 **/
int ExtendHashTableInitialize();


#endif //EXPERIMENT1_HASHTABLE_H
