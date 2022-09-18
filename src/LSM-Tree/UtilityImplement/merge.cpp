/**
 * @date     18/9/2022
 * @author   zz.wang
 * @details: This code is used to merge 
 *
 * IDENTIFICATION:
 *          src/LSM-Tree/UtilityDefine/merge.h
 **/

#include "../UtilityDefine/merge.h" // Related header file
#include <iostream>


void MergeContext::Insert(entry_t *entries, size_t EntryNum) 
{
    MergeEntryt MergeItem;

    if(EntryNum > 0) 
    {
        MergeItem.entries = entries;
        MergeItem.NumEntry = EntryNum;
        MergeItem.precedence = queue.size();
        queue.push(MergeItem);
    }
}

entry_t MergeContext::Contextpop(void) 
{
    MergeEntryt current, next;
    entry_t entry;

    current = queue.top();
    next = current;

    // Only release the most recent value for a given key
    while (next.head().key == current.head().key && !queue.empty()) 
    {
        queue.pop();

        next.Current++;
        if (!next.done()) 
        {
            queue.push(next);
        }
        next = queue.top();
    }
    return current.head();
}

bool MergeContext::IsEmpty(void) const 
{
    return queue.empty();
}
