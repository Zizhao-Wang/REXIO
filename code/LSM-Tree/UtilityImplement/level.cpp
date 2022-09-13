#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include "../UtilityDefine/level.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../Backend/IODisk/WriteDisk.h"

using namespace std;

Level::Level(long MaxRunSize)
{
	this->LevelNumber = LevelAlloctor();
	this->MaxRuns = pow(2,LevelNumber);
	this->MaxRunSize = MaxRunSize * MaxRuns;
}

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
            PagePointers[Size] = Pointer
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


std::vector<entry_t> Run::RunDataRead()
{


}

Run::~Run(void) 
{
    assert(mapping == nullptr);
}

entry_t * Run::map_read(size_t len, off_t offset) 
{
    assert(mapping == nullptr);

    mapping_length = len;

    mapping_fd = open(tmp_file.c_str(), O_RDONLY);
    assert(mapping_fd != -1);

    mapping = (entry_t *)mmap(0, mapping_length, PROT_READ, MAP_SHARED, mapping_fd, offset);
    assert(mapping != MAP_FAILED);

    return mapping;
}

entry_t * Run::map_read(void) 
{
    map_read(max_size * sizeof(entry_t), 0);
    return mapping;
}

entry_t * Run::map_write(void) 
{
    assert(mapping == nullptr);
    int result;

    mapping_length = max_size * sizeof(entry_t);

    mapping_fd = open(tmp_file.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
    assert(mapping_fd != -1);

    // Set the file to the appropriate length
    result = lseek(mapping_fd, mapping_length - 1, SEEK_SET);
    assert(result != -1);
    result = write(mapping_fd, "", 1);
    assert(result != -1);

    mapping = (entry_t *)mmap(0, mapping_length, PROT_WRITE, MAP_SHARED, mapping_fd, 0);
    assert(mapping != MAP_FAILED);

    return mapping;
}

void Run::unmap(void) 
{
    assert(mapping != nullptr);

    munmap(mapping, mapping_length);
    close(mapping_fd);

    mapping = nullptr;
    mapping_length = 0;
    mapping_fd = -1;
}

VAL_t * Run::get(KEY_t key) 
{
    vector<KEY_t>::iterator next_page;
    long page_index;
    VAL_t *val;
    int i;

    val = nullptr;

    if (key < fence_pointers[0] || key > max_key || !bloom_filter.is_set(key)) {
        return val;
    }

    next_page = upper_bound(fence_pointers.begin(), fence_pointers.end(), key);
    page_index = (next_page - fence_pointers.begin()) - 1;
    assert(page_index >= 0);

    map_read(getpagesize(), page_index * getpagesize());

    for (i = 0; i < getpagesize() / sizeof(entry_t); i++) {
        if (mapping[i].key == key) {
            val = new VAL_t;
            *val = mapping[i].val;
        }
    }

    unmap();

    return val;
}

vector<entry_t> * Run::range(KEY_t start, KEY_t end) 
{
    vector<entry_t> *subrange;
    vector<KEY_t>::iterator next_page;
    long subrange_page_start, subrange_page_end, num_pages, num_entries, i;

    subrange = new vector<entry_t>;

    // If the ranges don't overlap, return an empty vector
    if (start > max_key || fence_pointers[0] > end) {
        return subrange;
    }

    if (start < fence_pointers[0]) {
        subrange_page_start = 0;
    } else {
        next_page = upper_bound(fence_pointers.begin(), fence_pointers.end(), start);
        subrange_page_start = (next_page - fence_pointers.begin()) - 1;
    }

    if (end > max_key) {
        subrange_page_end = fence_pointers.size();
    } else {
        next_page = upper_bound(fence_pointers.begin(), fence_pointers.end(), end);
        subrange_page_end = next_page - fence_pointers.begin();
    }

    assert(subrange_page_start < subrange_page_end);
    num_pages = subrange_page_end - subrange_page_start;
    map_read(num_pages * getpagesize(), subrange_page_start * getpagesize());

    num_entries = num_pages * getpagesize() / sizeof(entry_t);
    subrange->reserve(num_entries);

    for (i = 0; i < num_entries; i++) {
        if (start <= mapping[i].key && mapping[i].key <= end) {
            subrange->push_back(mapping[i]);
        }
    }

    unmap();

    return subrange;
}

void Run::PutValue(entry_t entry) 
{
    assert(Size < MaxSize);

    Rundata.push_back(entry);    
    MaxKey = max(entry.key, max_key);
    if(Rundata.size() % )
    size++;
}
