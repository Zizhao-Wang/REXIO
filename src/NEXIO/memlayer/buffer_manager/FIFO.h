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
#include "include/node.h"


typedef struct FReadNode
{
	page_num_type   PageId;
	// key_value_entry * data;
}FReadNode;


class FIFOCache 
{

private:
    uint16_t capacity;
	std::list <FReadNode> Rcache;
	std::unordered_map<page_num_type, std::list<FReadNode>::iterator> HashMap;

public:
    FIFOCache(uint16_t cap) 
	{
        this->capacity = cap; 
    }
    
    // key_value_entry* get(page_num_type page); 
    void put(page_num_type page, FReadNode node);
	bool IsFIFOPage(page_num_type);
	void Clear(size_t cap);
};




#endif