#include "../UtilityDefine/run.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "../../Backend/IODisk/WriteDisk.h"

Run::Run(unsigned long maxsize)
{
    this->MaxSize = maxsize;
    for(int i=0;i<this->MaxSize/CalculatePageCapacity(sizeof(entry_t));i++) //Initialize all page pointers as UINT64_MAX
    {
        PagePointers.emplace_back(UINT64_MAX);
    }
    Size = 0;
}

int Run::RunDataWrite(void)
{

    uint64_t pagesize = CalculatePageCapacity(sizeof(entry_t));
    uint64_t Pointer;
    
    if(Rundata.size() == pagesize)
    {
        //printf("Rundata size: %lu Page number: %lu, Size: %lu\n",Rundata.size(),Pointer,Size);
        Pointer = SinglePageWrite(Rundata,PagePointers[Size/pagesize-1]);
        {
            printf("Rundata size: %lu Page number: %lu, Size: %lu\n",Rundata.size(),Pointer,Size);
            printf("Datum of Run in Level write succeed!\n");
            PagePointers[Size/pagesize-1] = Pointer;
            Rundata.clear();
            return 0;
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

std::vector<entry_t> Run::SingleRunRead()
{
    std::vector<entry_t> entries1; 
    size_t Pagecapacity = CalculatePageCapacity(sizeof(entry_t));

    printf("Test 1 : size of page pointers:%lu\n",PagePointers.size());
    for(size_t i=0; i<PagePointers.size();i++)
    {
        std::vector<entry_t> temp;
        temp = RunReadFromPage(PagePointers[i],Pagecapacity);
        entries1.insert(entries1.end(),temp.begin(),temp.end());
    }
    printf("Test 3");
    return entries1;

    /** entry_t * entries1 = nullptr; 
    entry_t * head = nullptr;
    size_t Pagecapacity = CalculatePageCapacity(sizeof(entry_t));
    size_t PageBytes = Pagecapacity * sizeof(entry_t);

    entries1 = RunReadFromPage(PagePointers[0],Pagecapacity);
    head = entries1;
    head += Pagecapacity;

    printf("Test 1");
    for(size_t i=1; i<PagePointers[i];i++)
    {
        head = RunReadFromPage(PagePointers[i],Pagecapacity);
        printf("Test 2");
        entries1  = (entry_t *)realloc(entries1, PageBytes+100);
        if(entries1 == NULL)
        {
            EMessageOutput("Memory allocating failure in SingleRunRead",578);
        }
        head = entries1 + Pagecapacity*(i+1);
    } **/
}

void Run::PutValue(entry_t entry) 
{
    assert(Size < MaxSize);
    Rundata.emplace_back(entry);
    Size++;    
    MaxKey = max(entry.key,MaxKey);
    if(Rundata.size() % CalculatePageCapacity(sizeof(entry_t)) == 0 && Size != 0)
    {
        //printf("Size in Put Value: %lu\n",Size);
        FencePointers.emplace_back(entry.key);
        int err = RunDataWrite();
        if(flag == 0)
        {
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

int Run::SetFencePointers(std::vector<uint64_t> pointers)
{
    size_t FirstSize = FencePointers.size();
    FencePointers.insert(FencePointers.end(),PagePointers.begin(),PagePointers.end());
    size_t MergeSize = FencePointers.size();
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

int Run::DataClear(std::vector<entry_t> MergeEntries)
{
    if(MultiPageWrite(MergeEntries,PagePointers) == -1)
    {
        return -1;
    } 
    return 0;
}

unsigned long Run::GetNowSize()
{
    return Size;
}

bool Run::Isfull(void)
{
    return GetNowSize() == MaxSize;
}