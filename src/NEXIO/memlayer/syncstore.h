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
#include "include/GlobalVariable.h"
#include "include/node.h"
#include "memlayer/buffer_manager/lru.h"
#include "memlayer/buffer_manager/FIFO.h"


/* Global variable declaration! */
extern int indexs;

extern uint32_t offset;








extern FIFOCache fifocache;

extern uint64_t sectors_per_page;

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
char* Read4Buffer(size_t Position);

char* SyncRead(uint32_t offset);

void clearBufferLog();

void countBufferLog();

#endif

