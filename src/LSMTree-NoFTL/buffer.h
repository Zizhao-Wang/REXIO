/**
 * @date:   1/3/2022
 * @author: zz.wang
 * @details:
 *          This is a header file for constructing a in-memory buffer(that equals Level 0 in LSM-Tree).
 *          There are two private variables in "buffer":
 *          std::set<entry_t> Entries and uint64_t MaxSize. 
 *          we constructed Entries for storing data passed from memory. 
 *          With key-values pairs insertig, different size of the set will be recorded. 
 *          The size must be less than or equal to a threshold called "MaxSzie".
 *
 * IDENTIFICATION:
 *          TiOCS/src/LSMTree-NoFTL/level0.h
 **/

#ifndef TiOCS_LSMTREENOFTL_BUFFER_H
#define TiOCS_LSMTREENOFTL_BUFFER_H

#include <set>
#include <vector>
#include "../Auxizilary/pre_definition.h"
#include "../Backend/IODisk/WriteDisk.h"
//#include "node2.h"


/*
 * Level 0 
 */
class memory_buffer 
{

private:
    uint64_t MaxSize;
    uint64_t size;
    std::set<entry_t> Entries;
    
public:
    int RandomLevel();
    memory_buffer(size_t maxpage);
    bool PutValue(KEY_t, VAL_t val);
    void display(); 
    VAL_t * GetValue(KEY_t);
    std::vector<entry_t> * GetRange(KEY_t, KEY_t) const;
    void AllClear(void);
    uint64_t GetMaxSize();
    std::vector<entry_t> GetEntries();
};

#endif //EXPERIMENT1_LSMTREE_BUFFER_H