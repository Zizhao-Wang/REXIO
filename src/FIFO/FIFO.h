/**
 * @date    16/10/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/FIFO/FIFO.h
 */

#ifndef EXPERIMENT1_SRC_FIFO_H
#define EXPERIMENT1_SRC_FIFO_H

#include <unordered_map>
#include <list>
#include "../Ti-OCSSD/MemoryTier/Node.h"


typedef struct FReadNode
{
	PageType   PageId;
	TNCEntry * data;
}FReadNode;


class FIFOCache 
{

private:
    uint16_t capacity;
	std::list <FReadNode> Rcache;
	std::unordered_map<PageType, std::list<FReadNode>::iterator> HashMap;

public:
    FIFOCache(uint16_t cap) 
	{
        this->capacity = cap; 
    }
    
    TNCEntry* get(PageType page); 
    void put(PageType page, FReadNode node);
	bool IsFIFOPage(PageType);
	void Clear(size_t cap);
};




#endif