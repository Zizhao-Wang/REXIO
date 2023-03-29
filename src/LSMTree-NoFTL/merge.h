/**
 * @date     6/3/2022
 * @author   zz.wang
 * @details 
 *           This code is used to merge 
 *
 * IDENTIFICATION:
 *          TiOCS/src/LSMTree-NoFTL/merge.h
 **/
#ifndef TiOCS_LSMTREENoFTL_MERGE_H
#define TiOCS_LSMTREENoFTL_MERGE_H

#include <cassert>  //C library file
#include <functional> // C++
#include <vector>
#include <queue>      // Library file
#include "../Auxizilary/pre_definition.h" //header files in this project

typedef struct merge_entry 
{

    entry_t SingleEntry;

    bool operator < (const merge_entry& other) const 
    {
        // Order first by keys, then by precedence
        if (SingleEntry.key == other.SingleEntry.key) 
        {
            return  SingleEntry.val < other.SingleEntry.val;
        } 
        else 
        {
            return SingleEntry.key > other.SingleEntry.key;
        }
    }

}merge_entry;

class merge_context 
{
    
private:
    std::priority_queue<merge_entry, std::vector<merge_entry>, std::less<merge_entry> > queue;

public:
    void Insert(entry_t *, size_t);
    void Insert(std::vector<entry_t> entries); 
    entry_t Contextpop(void);
    entry_t Contextpop1(); 
    bool IsEmpty(void) const;
    size_t get_size(void) const;
    
};

#endif