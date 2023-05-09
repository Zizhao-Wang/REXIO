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
#include "../../FIFO/FIFO.h"
#include "../../LOCS/io_scheduler.h"


/* Global variable declaration! */
extern int indexs;
extern uint32_t offset;
extern uint64_t sectors_per_page;
extern TNCEntry* Pagedata;
extern size_t buffer_size;
extern PageType WBufferId;
extern std::unordered_map<uint64_t, std::vector<char>> BufferLog;  //Log buffer 
extern LRUCache lrucache;
extern std::unordered_map<uint64_t, uint64_t> LogIndex;
extern int buffernumber;
// extern LRUCache lrucache;
extern FIFOCache fifocache;

/**
 *  ================= buffer manager module====================  
 **/
void buffer_init();

void buffer_cleanup();


/**
 *  ================= Synchronous write module====================  
 **/
uint32_t SyncWrite(const char* hashkey, const char* hashvalue);

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

