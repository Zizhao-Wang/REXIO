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
#include "../../Auxizilary/GlobalVariable.h"


Run::Run(long maxsize)
{
    this->MaxSize = maxsize;
    for(int i=0;i<this->MaxSize;i++) //Initialize all page pointers as UINT64_MAX
    {
        PagePointers.emplace_back(UINT64_MAX);
    }
    Size = 0;
}

int Run:: RunDataWrite(void)
{

    uint64_t pagesize = CalculatePageCapacity(sizeof(entry_t));
    uint64_t Pointer;
    
    printf("Rundata size: %lu Page size: %lu, Size: %lu\n",Rundata.size(),pagesize,Size);
    if(Rundata.size() == pagesize)
    {
        if(Pointer = PageDataWrite(Rundata,PagePointers[(Size/pagesize)-1]) != -1)
        {
            printf("Datum of Run in Level write succeed!\n");
            PagePointers[Size] = Pointer;
            Rundata.clear();
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
    RunReadFromPage(PageNum,CalculatePageCapacity(sizeof(entry_t)));
    return value;
}

entry_t* Run::SingleRunRead()
{
    entry_t * entries1 = nullptr; 
    entry_t * head = nullptr;
    size_t Pagecapacity = CalculatePageCapacity(sizeof(entry_t));
    size_t PageBytes = Pagecapacity * sizeof(entry_t);

    entries1 = RunReadFromPage(PagePointers[0],Pagecapacity);
    head = entries1;
    head += Pagecapacity;
    for(size_t i=1; i<PagePointers[i];i++)
    {
        head = RunReadFromPage(PagePointers[i],Pagecapacity);
        entries1  = (entry_t *)realloc(entries1, PageBytes+100);
        if(entries1 == NULL)
        {
            EMessageOutput("Memory allocating failure in SingleRunRead",578);
        }
        head = entries1 + Pagecapacity*(i+1);
    }
    
    return entries1;
}

void Run::PutValue(entry_t entry) 
{
    assert(Size < MaxSize);
    Rundata.push_back(entry);
    Size++;    
    MaxKey = max(entry.key,MaxKey);
    if(Size % CalculatePageCapacity(sizeof(entry_t)) == 0)
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

std::vector<uint64_t> Run::GetPagePointers(void)
{
    return PagePointers;
}

std::vector<KEY_t> Run::GetFencePointers(void)
{
    return FencePointers;
}

uint64_t Run::GetMaxKey(void)
{
    return MaxKey;
}


int Run::SetPagePointers(std::vector<uint64_t> pointers)
{
    size_t FirstSize = PagePointers.size();

    PagePointers.insert(PagePointers.end(),PagePointers.begin(),PagePointers.end());

    size_t MergeSize = PagePointers.size();

    if(MergeSize - FirstSize != pointers.size())
    {
        return -1;
    }

    return 0;
}

void Run::Unbind()
{
    PagePointers.clear();
    Rundata.clear();
    FencePointers.clear();
    Size = 0;
    AssertCondition(PagePointers.size()==0);
}

long Run::GetNowSize()
{
    return Size;
}

Level::Level(long buffersize)
{

    this->LevelNumber = LevelAlloctor();
	this->MaxRuns = 2;
	this->MaxRunSize = buffersize * pow(2,LevelNumber);
    for(int i=0;i<MaxRuns;i++)
    {
        Run run(MaxRunSize);
        Runs.emplace_back(run);
    }

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

long Level::GetLevelNumber(void) const
{
    return LevelNumber;
}