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

extern uint32_t offset2;

extern size_t buffer_capacity;

extern int buffernumber;

extern LRUCache lrucache;

extern PageType WBufferId;

extern TNCEntry* Pagedata;

extern char* key_buffer;

extern char *temp;

extern char* value_buffer;

extern char* log_data_buffer;

extern uint64_t block_allocator;

extern FIFOCache fifocache;

extern uint64_t sectors_per_page;

extern std::unordered_map<uint64_t, int> block_type_record;

extern std::unordered_map<uint64_t, uint64_t> LogIndex;

extern std::unordered_map<uint64_t, std::vector<char>> BufferLog;  //Log buffer 



/**
 *  ================= buffer manager module====================  
 **/
void buffer_init();

void kv_buffer_init();

void buffer_cleanup();

void kv_buffer_cleanup();


/**
 *  ================= Synchronous write module====================  
 **/
uint32_t SyncWrite(const char* hashkey, const char* hashvalue);

uint32_t SynckvseparateWrite(const char* hashkey, const char* hashvalue, uint32_t& block);

uint32_t SyncvseparateWrite(const char* hashvalue, uint32_t block);

uint32_t SynckvvariableseparateWrite(const char* hashkey, const char* hashvalue, uint32_t& block);

uint32_t SyncvvariableseparateWrite(const char* hashvalue, uint32_t block);


/**
 *  ================= Synchronous delete module====================  
 **/
int  SynckvvariableDelete(uint32_t offset);

int  SynckvDelete(uint32_t offset);

int  SyncvariableDelete(uint32_t offset);

int  SyncDelete(uint32_t offset);

/**
 *  ================= Synchronous read module====================  
 **/
TNCEntry Read4Buffer(size_t Position);

TNCEntry   SyncRead(uint32_t offset);

void clearBufferLog();

void countBufferLog();

#endif

