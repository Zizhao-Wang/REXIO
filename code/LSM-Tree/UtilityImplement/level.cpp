#include <cassert>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include "../UtilityDefine/level.h"


Run::Run(long maxsize)
{
    this->MaxSize = maxsize;
    //Initialize all page pointers as  uint32_MAX
    for(int i=0;i<this->MaxSize;i++)
    {
        PagePointers.emplace_back(UINT64_MAX);
    }
    Size = 0;
}

int Run::RunDataWrite(void)
{

    uint64_t pagesize = CalculatePageCapacity(sizeof(KEY_t)+sizeof(VAL_t));
    uint64_t Pointer;
    if(pagesize == Rundata.size())
    {
        if(Pointer = PageDataWrite(Rundata,PagePointers[Size]) != -1)
        {
            printf("Datum of Run in Level write succeed!\n");
            PagePointers[Size] = Pointer;
            return 0;
        }
        else
        {
            printf("Datum of Run in Level write failed!\n");
            return -1;
        }
    }
    
    printf("Datum size of Run doesn't matching, check again!");
    return -1;

}


VAL_t* Run::RunValuesRead(uint64_t PageNum)
{
    VAL_t * value = nullptr;
    std::vector<entry_t> reads;
    reads = RunReadFromPage(PageNum);
    return value;
}

entry_t* Run::SingleRunRead()
{
    entry_t * entries;
    return entries;

}

void Run::PutValue(entry_t entry) 
{
    assert(Size < MaxSize);
    Rundata.push_back(entry);    
    MaxKey = max(entry.key,MaxKey);
    if(Size % CalculatePageCapacity(sizeof(KEY_t)+sizeof(VAL_t)) == 0)
    {
        FencePointers.emplace_back(entry.key);
        int err = RunDataWrite();
        if(flag == 0)
        {
            Rundata.clear();
            assert(Rundata.size() == 0); 
        }
        else
        {
            EMessageOutput("Run failed!",104);
        }
    }
    Size++;
}

VAL_t * Run::GetValue(KEY_t key)  
{
    std::vector<KEY_t>::iterator NextPage;
    long PageIndex;
    VAL_t * value = nullptr;

    if (key < FencePointers[0] || key > MaxKey) 
    {
        return value;
    }

    NextPage = upper_bound(FencePointers.begin(), FencePointers.end(), key);
    PageIndex = (NextPage - FencePointers.begin()) - 1;
    assert(PageIndex >= 0);

    value = RunValuesRead(PagePointers[PageIndex]);

    return value;
}




// vector<entry_t> * Run::range(KEY_t start, KEY_t end) 
// {
//     vector<entry_t> *subrange;
//     vector<KEY_t>::iterator next_page;
//     long subrange_page_start, subrange_page_end, num_pages, num_entries, i;

//     subrange = new vector<entry_t>;

//     // If the ranges don't overlap, return an empty vector
//     if (start > max_key || fence_pointers[0] > end) {
//         return subrange;
//     }

//     if (start < fence_pointers[0]) {
//         subrange_page_start = 0;
//     } else {
//         next_page = upper_bound(fence_pointers.begin(), fence_pointers.end(), start);
//         subrange_page_start = (next_page - fence_pointers.begin()) - 1;
//     }

//     if (end > max_key) {
//         subrange_page_end = fence_pointers.size();
//     } else {
//         next_page = upper_bound(fence_pointers.begin(), fence_pointers.end(), end);
//         subrange_page_end = next_page - fence_pointers.begin();
//     }

//     assert(subrange_page_start < subrange_page_end);
//     num_pages = subrange_page_end - subrange_page_start;
//     map_read(num_pages * getpagesize(), subrange_page_start * getpagesize());

//     num_entries = num_pages * getpagesize() / sizeof(entry_t);
//     subrange->reserve(num_entries);

//     for (i = 0; i < num_entries; i++) {
//         if (start <= mapping[i].key && mapping[i].key <= end) {
//             subrange->push_back(mapping[i]);
//         }
//     }

//     unmap();

//     return subrange;
// }

void Run::Unbind()
{
    PagePointers.clear();
    AssertCondition(PagePointers.size()==0);
}

long Run::GetNowSize()
{
    return Size;
}

Level::Level(long buffersize)
{
	this->MaxRuns = 2;
	this->MaxRunSize = buffersize * pow(2,LevelAlloctor());
    
}

bool Level::IsEmpty(void) const
{
    return Runs.size()==0;
}

bool Level::IsFull(void) const
{
    return Runs.size()== MaxRunSize;
}

long Level::GetMRunSize(void) const
{
    return MaxRunSize;
}