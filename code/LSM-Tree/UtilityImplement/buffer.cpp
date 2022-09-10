#include <iostream>
#include "../UtilityDefine/buffer.h"


VAL_t * Buffer::GetValue(KEY_t key) const 
{
    entry_t SearchEntry;
    std::set<entry_t>::iterator EntryItor;
    VAL_t *value;

    SearchEntry.key = key;
    EntryItor = Entries.find(SearchEntry);

    if (EntryItor == Entries.end()) 
    {
        return nullptr;
    } else 
    {
        value = new VAL_t;
        (*value) = EntryItor->val;
        return value;
    }
}

std::vector<entry_t> * Buffer::GetRange(KEY_t start, KEY_t end) const 
{
    entry_t SearchEntry;
    std::set<entry_t>::iterator SubRangeStart, SubRangeEnd;

    SearchEntry.key = start;
    SubRangeStart = Entries.lower_bound(SearchEntry);

    SearchEntry.key = end;
    SubRangeEnd = Entries.upper_bound(SearchEntry);

    return new std::vector<entry_t>(SubRangeStart, SubRangeEnd);
}

bool Buffer::put(KEY_t key, VAL_t val) 
{
    entry_t SingleEntry;
    std::set<entry_t>::iterator itor;
    bool found;

    if (Entries.size() == MaxSize) 
    {
        return false;
    } 
    else 
    {
        std::pair<std::set<entry_t>::iterator,bool> Tempair;
        SingleEntry.key = key;
        SingleEntry.val = val;
        Tempair = Entries.insert(SingleEntry);
        
        if (found == false)  // Update the entry if it already exists
        {
            Entries.erase(itor);
            Entries.insert(SingleEntry);
        }
        return true;
    }
}

void Buffer::empty(void) 
{
    Entries.clear();
}
