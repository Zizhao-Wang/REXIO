/**
 * @date     18/9/2022
 * @author   zz.wang
 * @details: This code is used to merge 
 *
 * IDENTIFICATION:
 *          src/LSM-Tree/UtilityDefine/merge.h
 **/
#ifndef EXPERIMENT1_MERGE_H
#define EXPERIMENT1_MERGE_H

#include <cassert>  //C library file
#include <functional> // C++
#include <queue>      // Library file
#include "../../Auxizilary/VariablesTypes.h" //header files in this project

typedef struct MergeEntry 
{
    int precedence; //priority
    entry_t *entries;
    long NumEntry;
    int Current = 0;
    
    entry_t head(void) const 
    {
        return entries[Current];
    }

    bool done(void) const 
    {
        return Current == NumEntry;
    }

    bool operator > (const MergeEntry& other) const 
    {
        // Order first by keys, then by precedence
        if (head() == other.head()) 
        {
            assert(precedence != other.precedence);
            return precedence > other.precedence;
        } 
        else 
        {
            return head() > other.head();
        }
    }

}MergeEntryt;

class MergeContext 
{
private:

    std::priority_queue<MergeEntryt, std::vector<MergeEntryt>, std::greater<MergeEntryt>> queue;

public:
    void Insert(entry_t *, size_t);
    entry_t Contextpop(void);
    bool IsEmpty(void) const;
    
};

#endif