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
#include "../TNCTree/MemoryTier/Node.h"


typedef struct ReadNode
{
	PageType   PageId;
	TNCEntry * data;
}ReadNode;


class LRUCache 
{

private:
    uint16_t capacity;
    std::list<ReadNode> cache;
    // 哈希表：key 映射到 (key, value) 在 cache 中的位置
    std::unordered_map<int, std::list<ReadNode>::iterator> HashMap;

public:
    LRUCache(uint16_t cap) 
	{
        this->capacity = cap; 
    }
    
    TNCEntry* get(PageType page); 
    void put(PageType page, ReadNode node);
	bool IsLRUPosition(void);
	bool IsLRUFull(void);
	bool IsLRUPage(PageType);

};



extern std::unordered_map<uint64_t, ReadNode*> ReadBuffer;

int LRUPut(TNCEntry *);

int LRUGet(PageType );



#endif