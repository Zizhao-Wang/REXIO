#include <cassert>
#include <iostream>
#include "../UtilityDefine/merge.h"

void MergeContext::Insert(entry_t *entries, size_t EntryNum) 
{
    MergeEntryt merge_entry;

    if (EntryNum > 0) 
    {
        merge_entry.entries = entries;
        merge_entry.NumEntry = EntryNum;
        merge_entry.precedence = queue.size();
        queue.push(merge_entry);
    }
}

entry_t MergeContext::Contextpop(void) 
{
    merge_entry_t current, next;
    entry_t entry;

    current = queue.top();
    next = current;

    // Only release the most recent value for a given key
    while (next.head().key == current.head().key && !queue.empty()) 
    {
        queue.pop();

        next.current_index++;
        if (!next.done()) queue.push(next);

        next = queue.top();
    }

    return current.head();
}

bool MergeContext::IsEmpty(void) const 
{
    return queue.empty();
}
