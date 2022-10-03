/**
 * @date    2/10/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/StoreTier/asnycstore.h
 */

#ifndef EXPERIMENT1_TNCTREE_SYNCSTORE_H
#define EXPERIMENT1_TNCTREE_SYNCSTORE_H

#include <stdint.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "../../Auxizilary/GlobalVariable.h"
#include "../MemoryTier/Node.h"
#include "../../LRU/lru.h"

/* Global variable declaration! */
extern int indexs;
extern uint32_t offset;
//extern TNCEntry * Pagedata;
extern TNCEntry Pagedata[1030];
extern PageType WBufferId;
extern std::unordered_map<uint64_t, std::vector<char>> BufferLog;  //Log buffer 
extern LRUCache lrucache;

/**
 *  ================= Synchronous write module====================  
 **/
uint32_t SyncWrite(SKey key1, SValue value);

/**
 *  ================= Synchronous delete module====================  
 **/
int      SyncDelete(uint32_t offset);

/**
 *  ================= Synchronous read module====================  
 **/
TNCEntry Read4Buffer(size_t Position);

TNCEntry   SyncRead(uint32_t offset);

#endif

