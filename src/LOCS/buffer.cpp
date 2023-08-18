#include "buffer.h"
#include <iostream>
#include "global_variables.h"
#include "locs.h"
#include "../Debug/debug_micros.h"
#include "../Backend/IODisk/WriteDisk.h"

const int MAX_LEVEL = 32;
const int P_FACTOR = RAND_MAX >> 2;

locs_buffer::locs_buffer(size_t max_chunks)
{

    this->MaxSize =max_chunks * ((geometry.clba/64)*(( SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES* page_size) / sizeof(entry_t)));
    // this->MaxSize = 4;
    printf("MaxSize:%lu\n",MaxSize);
    size = 0;
    //printf("key:%lu \n\n",Head->head->key);
}

int locs_buffer::RandomLevel()
{

    int v = 1;
    while (rand() < P_FACTOR && v < MAX_LEVEL) {
        v++;
    }
#ifdef DEBUG

#endif
    return v;

}

void locs_buffer::display()
{
    return ;
}

bool locs_buffer::PutValue(const char* key, const char* value) 
{
#ifdef SET
    SkiplistNode * update[MAX_LEVEL];
    SkiplistNode * curr = Head->head;
    if (size >= MaxSize) 
    {
        return false;
    } 
    else 
    {
        for(int i=Head->level-1; i>=0; i--)
        {
            while(curr->forward[i] && curr->forward[i]->key < key1)
            {
                curr = curr->forward[i];
            }
            update[i] = curr;
        }
        if(curr->key == key1)
        {
        
            curr->key = key1;
            curr->value = val1;
            return true;
        }
        else
        {
            int v=RandomLevel();
            if(v > Head->level)
            {
                for(int i=Head->level; i<v; ++i )
                {
                    update[i] = Head->head;
                }
                Head->level = v;
            }

            SkiplistNode * lst = skiplistNodeCreat(key1,val1,v);
            for(int i=0;i<v;i++)
            {
                lst->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = lst;
            }
            size++;
        }
        return true;
    }
#endif
    if (size >= MaxSize) 
    {
        return false;
    }

    entry_t SingleEntry;
    memcpy(SingleEntry.key, key, KEY_SIZE);
    memcpy(SingleEntry.val, value, VAL_SIZE);

    std::pair<std::set<entry_t>::iterator,bool> Tempair;
    size++;
    Tempair = Entries.insert(SingleEntry);
    if (Tempair.second == false)  // Update the entry if it already exists
    {
        Entries.erase(Tempair.first);
        Entries.insert(SingleEntry);
        size--;
    }

    return true;   
    
}

const char* locs_buffer::GetValue(const char* key)
{

#ifdef SKIPLIST1
    SkiplistNode *curr = Head->head;
    for (int i = Head->level - 1; i >= 0; i--) 
    {
        while (curr->forward[i] && curr->forward[i]->key < key1) 
        {
            curr = curr->forward[i];
        }
    }
    curr = curr->forward[0];
    if (curr && curr->key ==key1 ) 
    {
        VAL_t * value = new VAL_t;
        *value = curr->value;
        return value;
    } 
    return nullptr;    
#endif
    
#ifdef RED_BLACK_TREE

    entry_t search_entry;
    set<entry_t>::iterator entry;

    memcpy(search_entry.key, key, KEY_SIZE);
    entry = Entries.find(search_entry);

    if (entry == Entries.end()) 
    {
        return nullptr;
    }  
    char *value = new char[VAL_SIZE];
    memcpy(value, entry->val, VAL_SIZE);
    return value;

#endif
}

std::vector<entry_t> * locs_buffer::GetRange(const char* start, const char* end) const 
{
    entry_t SearchEntry;
    std::set<entry_t>::iterator SubRangeStart, SubRangeEnd;

    // SearchEntry.key = start;
    // SubRangeStart = Entries.lower_bound(SearchEntry);

    // SearchEntry.key = end;
    // SubRangeEnd = Entries.upper_bound(SearchEntry);

    return new std::vector<entry_t>(SubRangeStart, SubRangeEnd);
}


void locs_buffer::AllClear(void) 
{
#ifdef SKIPlist
    for (SkiplistNode * curr = Head->head; curr; ) 
    {
        SkiplistNode *prev = curr;
        curr = curr->forward[0];
        skiplistNodeFree(prev);
    }
    Head = skiplistCreate();
#endif

    Entries.clear();
    size = 0;

}

uint64_t locs_buffer::GetMaxSize()
{
    return MaxSize;
}

std::vector<entry_t> locs_buffer::GetEntries()
{
#ifdef sk
    std::vector<entry_t> values;
     SkiplistNode * first = Head->head;

    while (first->forward[0]!=NULL)
    {
        if(first->key !=0)
        {
            entry_t en;
            en.key = first->key;
            en.val = first->value;
            values.emplace_back(en);
        }
        first = first->forward[0];        /* code */
    }
    entry_t en;
    en.key = first->key;
    en.val = first->value;
    values.emplace_back(en);

    return values;
#endif
    std::vector<entry_t> valus;
    std::set<entry_t>::iterator it;
    for(it = Entries.begin();it != Entries.end(); it++)
    {
        valus.emplace_back(*it);
    }
    return valus;
}
