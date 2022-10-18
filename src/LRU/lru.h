/**
 * @date    3/10/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/StoreTier/LRU.h
 */

#ifndef EXPERIMENT1_TNCTREE_LRU_H
#define EXPERIMENT1_TNCTREE_LRU_H

#include <vector>
#include <unordered_map>
#include <list>
#include <cassert>
#include "../TNCTree/MemoryTier/Node.h"


typedef struct ReadNode
{
	PageType   PageId;
	TNCEntry * data;
}ReadNode;


class LRUCache 
{

private:
    
    
    std::unordered_map<int, std::list<ReadNode>::iterator> HashMap;

public:
    size_t capacity;
    std::list<ReadNode> cache;
    LRUCache(size_t cap) 
	{
        this->capacity = cap; 
    }
    
    TNCEntry* get(PageType page); 
    void put(PageType page, ReadNode node);
	bool IsLRUPage(PageType);
    void ClearaReset(size_t cap);

};




#endif