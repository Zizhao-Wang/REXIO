/**
 * @date:   8/9/2022
 * @author: zz.wang
 * @details:
 *          This file shows how to construct a disk-based LSM-Tree.
 *
 * IDENTIFICATION:
 *          src/LSM-Tree/UtilityDefine/buffer.h
 **/

#ifndef EXPERIMENT1_LSMTREE_BUFFER_H
#define EXPERIMENT1_LSMTREE_BUFFER_H

#include <set>
#include <vector>
#include "../../Auxizilary/VariablesTypes.h"
#include "../../Backend/IODisk/WriteDisk.h"

/*
 * Level 0 
 */
class Buffer 
{
private:
    uint64_t MaxSize;
    std::set<entry_t> Entries;

public:
    Buffer(size_t maxpage);
    bool PutValue(KEY_t, VAL_t val);
    VAL_t * GetValue(KEY_t) const;
    std::vector<entry_t> * GetRange(KEY_t, KEY_t) const;
    void AllClear(void);
    uint64_t GetMaxSize();
    std::vector<entry_t> GetEntries();
};

#endif //EXPERIMENT1_LSMTREE_BUFFER_H