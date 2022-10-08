#include <iostream>
#include "../UtilityDefine/buffer.h"
#include "../LsmTree.h"

Buffer::Buffer(size_t maxpage)
{
    // 1024 * 256 = 262144 
    uint64_t capacity =  CalculatePageCapacity(sizeof(entry_t));
    this->MaxSize = capacity * maxpage;
    size = 0;
    //printf("Test successful! Size of entry:%lu, Page capacity: %lu, Buffer size:%u\n",sizeof(entry_t),capacity,MaxSize);
}

int Buffer::RandomLevel()
{

    int v = 1;
    srand ((unsigned int)(time (NULL)));
    while(rand() % (N + 1) / (float)(N + 1) < pro && v< MaxLevel )
    {
        v++;
    }
#ifdef DEBUG

#endif
    return v;

}


bool Buffer::PutValue(KEY_t key1, VAL_t val1) 
{
    entry_t SingleEntry;
    std::set<entry_t>::iterator itor;
    LSMEntry * update[20];
    LSMEntry * temp = head->Node;

    if (size >= MaxSize) 
    {
        return false;
    } 
    else 
    {
        int curLevel = head->CurrentLevel-1;

        for(int i=curLevel; i>=0; --i)
        {
            while(temp->next[i]->key < key1)
            {
                temp = temp->next[i];
            }
            update[i] = temp;
        }
    temp = temp->next[0];

    if(temp->key == key1)
    {
        
        temp->key = key1;
            // write into disk
        temp->val = val1;
        ++head->Nodenumber;
        return 0;
        
    }
    else
    {
        int v=RandomLevel();
        if(v > curLevel)
        {
            for(int i=curLevel+1; i<v; ++i )
            {
                update[i] = head->HashNode;
            }
            head->CurrentLevel = v+1;
        }
        //write into disk
        uint_32 offset1 = SyncWrite(hashkey,hashvalue);  //printf("%u\n",temp->offset);
        ++head->Nodenumber;
        temp = Initialization(hashkey,offset1);
        if(temp == nullptr)
            return -1;
        for(int i=0;i<v;++i)
        {
            temp->next[i] = update[i]->next[i];
            update[i]->next[i] = temp;
        }
    }

#ifdef SET
        std::pair<std::set<entry_t>::iterator,bool> Tempair;
        SingleEntry.key = key;
        SingleEntry.val = val;
        Tempair = Entries.insert(SingleEntry);
        //printf("Key:%lu Value:%lu Result: %d\n",key,val,Tempair.second);
        if (Tempair.second == false)  // Update the entry if it already exists
        {
            Entries.erase(itor);
            Entries.insert(SingleEntry);
        }
#endif
        return true;
    }
}

VAL_t * Buffer::GetValue(KEY_t key)
{
    entry_t SearchEntry;
    std::set<entry_t>::iterator EntryItor;
    VAL_t *value;

    SearchEntry.key = key;
    EntryItor = Entries.find(SearchEntry);

    if (EntryItor == Entries.end()) 
    {
        return nullptr;
    } else {
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


void Buffer::AllClear(void) 
{
    Entries.clear();
}

uint64_t Buffer::GetMaxSize()
{
    return MaxSize;
}

std::vector<entry_t> Buffer::GetEntries()
{
    std::vector<entry_t> values;
    for(auto entry : Entries)
    {
        values.emplace_back(entry);
    }
    return values;
}
