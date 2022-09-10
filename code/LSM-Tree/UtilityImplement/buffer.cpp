#include <iostream>
#include "../UtilityDefine/buffer.h"


VAL_t * Buffer::GetValue(KEY_t key) const 
{
    entry_t SearchEntry;
    std::set<entry_t>::iterator EntryItor;
    VAL_t *value;

    SearchEntry.key = key;
    EntryItor = Entries.find(SearchEntry);

    if (EntryItor == entries.end()) 
    {
        return nullptr;
    } else 
    {
        value = new VAL_t;
        (*value) = entry->val;
        return value;
    }
}

vector<entry_t> * Buffer::range(KEY_t start, KEY_t end) const {
    entry_t search_entry;
    set<entry_t>::iterator subrange_start, subrange_end;

    search_entry.key = start;
    subrange_start = entries.lower_bound(search_entry);

    search_entry.key = end;
    subrange_end = entries.upper_bound(search_entry);

    return new vector<entry_t>(subrange_start, subrange_end);
}

bool Buffer::put(KEY_t key, VAL_t val) {
    entry_t entry;
    set<entry_t>::iterator it;
    bool found;

    if (entries.size() == max_size) {
        return false;
    } else {
        entry.key = key;
        entry.val = val;

        tie(it, found) = entries.insert(entry);

        // Update the entry if it already exists
        if (found == false) {
            entries.erase(it);
            entries.insert(entry);
        }

        return true;
    }
}

void Buffer::empty(void) {
    entries.clear();
}
