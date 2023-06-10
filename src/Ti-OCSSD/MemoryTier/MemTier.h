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
bool LocalGeneration(GlobalHashNode * global, int initialLocalDepth);

bool DoubleHashtable();

uint64_t big_endian2little_endian(const char *big_endian, size_t len);


/**  
 * ================= Insertion module ====================  
 **/

int RandomLevel();

void NodeSplit(size_t bucket_index);

#ifdef NOT_SEPARATE_KV
int InsertNode(const char* hashkey, uint32_t offset, uint8_t flag, int bucket_id);
#else
int InsertNode(const char* hashkey, uint32_t offset, uint8_t flag,uint8_t block, int bucket_id);
#endif

int InsertNode(const char* hashkey, const char* hashvalue);


/**
 *  =================  Search module  ==================== 
 **/

TSkiplistNode * SearchNode(TNCSkiplist * Head, const char*  hashkey);

SValue Search(const char* );


/**  
 * ================= update module ====================  
 **/
int Update(const char* key1, const char* val);


/**
 *  ================= deletion module====================  
 **/
bool DeleteValue(TNCSkiplist * Head, const char* hashkey);
int Delete(const char* key1);


/**  
 * ================= Memory-tier initialization module ====================  
 **/
int ExtendHashTableInitialize();


#endif //EXPERIMENT1_HASHTABLE_H
