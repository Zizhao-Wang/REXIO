/**
 * @date     18/9/2022
 * @author   zz.wang
 * @details: This code is used to merge 
 *
 * IDENTIFICATION:
 *          src/LSM-Tree/UtilityDefine/merge.h
 **/

#include "merge.h" // Related header file
#include <iostream>


void merge_context::Insert(entry_t *entries, size_t EntryNum) 
{
    merge_entry MergeItem;
    size_t index = 0;
    while(index < EntryNum) 
    {
        MergeItem.SingleEntry = entries[index];
        queue.push(MergeItem);
        index++;
    }
    free(entries);
}

void merge_context::Insert(std::vector<entry_t> entries) 
{
    
    merge_entry MergeItem;
    size_t index = 0;
    
    // printf("lls: Inserted key: %lu\n", test(entries[entries.size() - 1].key));
    
    while(index < entries.size()) 
    {
        MergeItem.SingleEntry = entries[index];
        queue.push(MergeItem);
        index++;

        uint64_t key_value = test(MergeItem.SingleEntry.key);
#ifdef BIG_TO_LITTLE_ENDIAN        
        if (index == 1 || index == entries.size() || key_value == 1048576 || key_value == 1572864)
        {
            printf("Inserted key: %lu\n", key_value);
        }
#endif
    }
    //printf("priority_queue size: %lu \n",queue.size());

}
void merge_context::Insert(entry_t entrie)
{
    merge_entry MergeItem;
    MergeItem.SingleEntry = entrie;
    queue.push(MergeItem);
}



entry_t merge_context::Contextpop() 
{
    // printf("priority_queue.top():%lu",queue.top().SingleEntry.key);
    // exit(0);
    merge_entry current, next;
    entry_t entry;

    current = queue.top();
    next = current;

    while ( test(next.SingleEntry.key) == test(current.SingleEntry.key) && !queue.empty())  // Only release the most recent value for a given key
    {
        queue.pop();
        next = queue.top();
    }

    return current.SingleEntry;
}

entry_t merge_context::Contextpop1() 
{
    // printf("priority_queue.top():%lu",queue.top().SingleEntry.key);
    // exit(0);
    merge_entry current;

    current = queue.top();

    if(!queue.empty())  // Only release the most recent value for a given key
    {
        queue.pop();
    }

    return current.SingleEntry;
}

bool merge_context::IsEmpty(void) const 
{
    return queue.empty();
}

size_t merge_context::get_size(void) const 
{
    return queue.size();
}

void  merge_context::clear(void)
{
    while (!queue.empty())
    {
        queue.pop();
    }
}