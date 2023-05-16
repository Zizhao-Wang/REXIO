#include "run.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "../Backend/IODisk/WriteDisk.h"
#include "../Backend/SSDWrite/writer.h"
#include "../Backend/SSDRead/reader.h"
#include "global_variables.h"
#include "io_scheduler.h"

#include "../Debug/debug_micros.h"

vector<int> a;

locs_run::locs_run(unsigned long maxsize)
{
    this->MaxSize = maxsize;
    this->max_io_size = get_max_data_entries_per_io();
    memset(this->max_key, 0, KEY_SIZE);
    memset(this->min_key, 0xFF, KEY_SIZE);
    this->Size = 0;
    this->io_count = 0;
    this->io_time_reord = 0;

    this->max_entries_per_vector = geometry.clba * page_size / sizeof(entry_t);
    this->current_vector_index = 0;

    size_t num_vectors = MaxSize /chunk_capacity;
    Rundata.emplace_back(std::vector<entry_t>());
}

void locs_run::PointersDisplay()
{
    for(size_t i=0;i<chunk_pointers.size();i++)
    {
        printf("page pointer:%lu\n",chunk_pointers[i]);
    }
}


uint64_t locs_run::RunDataWrite(size_t index)
{
    return select_write_queue(Rundata[index], OCSSD_WRITE); 
}

std::vector<entry_t> locs_run::SingleRunRead()
{
    std::vector<entry_t> entries1; 
    
    for(size_t i=0; i<chunk_pointers.size();i++)
    {
        for(size_t j=chunk_pointers[i]*geometry.clba;j<chunk_pointers[i]*geometry.clba+geometry.clba;j+=64)
        {
            std::vector<entry_t> temp;
            temp = select_read_queue(j, OCSSD_READ);
            if(temp.size()!=0)
            {
                entries1.insert(entries1.end(),temp.begin(),temp.end());
            }
        }
    }
    return entries1;
}

void locs_run::PutValue(entry_t entry) 
{
    assert(Size < MaxSize);

    if(Rundata[current_vector_index].size() == max_entries_per_vector)
    {
        current_vector_index++;
        Rundata.emplace_back(std::vector<entry_t>());
    }

    Rundata[current_vector_index].emplace_back(entry);
    
    

    if (memcmp(entry.key, max_key, KEY_SIZE) > 0) 
    {
        memcpy(max_key, entry.key, KEY_SIZE);
    }

    if (memcmp(entry.key, min_key, KEY_SIZE) < 0) 
    {
        memcpy(min_key, entry.key, KEY_SIZE);
    }

    Size++;
    
    if(Size% max_io_size ==0 && Size!=0)
    {
        char * key =new char[KEY_SIZE];
        memcpy(key,entry.key,KEY_SIZE);
        FencePointers.emplace_back(key);
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    if (Size == MaxSize) 
    {

        size_t total_vectors = Rundata.size();
      

        for (size_t start = 0; start < total_vectors; start++) 
        {
            uint64_t pointer =  RunDataWrite(start);
            chunk_pointers.emplace_back(pointer);
        }
        // auto end_time = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        // std::cout << "Total I/O time for current batch of threads: " << duration.count() << "ms\n";

        current_vector_index = 0;
        Rundata.clear();
        Rundata.emplace_back(std::vector<entry_t>());
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    time_record += duration.count();

}


bool compare_keys(const char *a, const char* b) 
{
    return memcmp(a, b, KEY_SIZE) < 0;
}

std::vector<char*>::iterator lowerBoundManual(std::vector<char*>& vec, const char* key, int key_len) 
{
    int l = 0;
    int r = vec.size();
    while (l < r) 
    {
        int mid = l + (r - l) / 2;
        if (memcmp(vec[mid], key, key_len) < 0) 
        {
            l = mid + 1;
        }
        else 
        {
            r = mid;
        }
    }
    return vec.begin() + l;
}

const char * locs_run::GetValue(const char* key)  
{
    if(Size == 0)
        return nullptr;
    char * value = nullptr;
    std::vector<entry_t> reads;

    //printf("Run size:%lu\n min:%lu max:%lu\n",Size,min_key,max_key);
    if(FencePointers.size()!=0)
    {
        if (memcmp(key, min_key, KEY_SIZE) < 0 || memcmp(key, max_key, KEY_SIZE) > 0) 
        {
            return nullptr;
        }
    }

#ifdef DEBUG_SEARCH
    printf("key in get in FencePointers: \n");
    for (const auto& ptr : FencePointers) 
    {
        if (ptr != nullptr) 
        {
            printf(" %lu ", test(ptr));
        }
    }
    printf("======\n");
#endif

    std::vector<char*>::iterator it = lowerBoundManual(FencePointers,key,KEY_SIZE);
    size_t PageIndex = it-FencePointers.begin();
    
    entry_t search_example;
    memcpy(search_example.key,key,KEY_SIZE);
    memset(search_example.val,0,VAL_SIZE);

    uint64_t max_io_chunk = (geometry.clba /SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES);
    if( PageIndex/max_io_chunk < chunk_pointers.size() )
    {
        
        reads = select_read_queue(chunk_pointers[PageIndex/max_io_chunk]*geometry.clba+(PageIndex%max_io_chunk)*max_io_chunk, OCSSD_READ);

#ifdef DEBUG_SEARCH
        /* printf debug parameters*/
        printf("chunk_pointers[PageIndex/max_io_chunk]+(PageIndex%max_io_chunk)*max_io_chunk %lu\n",chunk_pointers[PageIndex/max_io_chunk]+(PageIndex%max_io_chunk)*max_io_chunk);
#endif

        std::vector<entry_t>::iterator get;
        get = find(reads.begin(),reads.end(),search_example);
        if(get!=reads.end())
        {
            value = (*get).val;
            return value; 
        }
        
    }
    else
    {
        std::vector<entry_t>::iterator get;
        // get = find(Rundata.begin(),Rundata.end(),search_example);
        // if(get!=reads.end())
        // {
        //     value = (*get).val;
        //     return value; 
        // }
    }
    
    delete(value);
    return nullptr;
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

std::vector<uint64_t> locs_run::GetPagePointers(void)
{
    return chunk_pointers;
}

std::vector<char*> locs_run::GetFencePointers(void)
{
    // for(size_t k=0;k<FencePointers.size();k++)
    // {
    //     printf("  %lu in FlushInto ", test(FencePointers[k]));
    // }
    return FencePointers;
}

const char * locs_run::get_max_key(void)
{
    return max_key;
}

const char * locs_run::get_min_key(void)
{
    return min_key;
}


int locs_run::set_chunk_pointers(std::vector<PageType> pointers)
{
    assert(chunk_pointers.size()==0);
    chunk_pointers.insert(chunk_pointers.end(),pointers.begin(),pointers.end());

    return 0;
}

int locs_run::SetFencePointers(std::vector<char*> pointers)
{

    for(size_t i=0;i<pointers.size();++i)
    {
        FencePointers.emplace_back(pointers[i]);
    }
    return 0;
}

void locs_run::Reset()
{
    if(FencePointers.size()!=0)
        FencePointers.clear();

    Size = 0;
    
    if(Rundata.size()!=0)
    {
        Rundata.clear();
        Rundata.emplace_back(std::vector<entry_t>());
    }
        

    if(chunk_pointers.size()!=0)
    {
        uint64_t channel_id  = 0;
        for(size_t i=0;i<chunk_pointers.size();++i)
        {
            channel_id = chunk_pointers[i] / (geometry.num_chk * geometry.num_pu);
            channels[channel_id].chunk_type[chunk_pointers[i] % (geometry.num_chk * geometry.num_pu)] = DISUSED_CHUNK;
        }
        chunk_pointers.clear();
    }

    memset(max_key, 0, KEY_SIZE);
    memset(min_key, 0xFF, KEY_SIZE);

    io_count = 0;

    current_vector_index = 0;
}

void locs_run::set_max_key(const char* key)
{
    if (memcmp(key, max_key, KEY_SIZE) > 0) 
    {
        memcpy(max_key, key, KEY_SIZE);
    }
}

void locs_run::set_min_key(const char* key)
{
    if (memcmp(key, min_key, KEY_SIZE) < 0) 
    {
        memcpy(min_key, key, KEY_SIZE);
    }
}

void locs_run::set_current_size(size_t current_size)
{
    Size = current_size;
}

void locs_run::Unbind()
{
    chunk_pointers.clear();
    Rundata.clear();
    FencePointers.clear();
    Size = 0;
    AssertCondition(chunk_pointers.size()==0);
}

int locs_run::DataClear(std::vector<entry_t> MergeEntries)
{
    // if(MultiPageWrite(MergeEntries,PagePointers) == -1)
    // {
    //     return -1;
    // } 
    return 0;
}

unsigned long locs_run::GetNowSize()
{
    return Size;
}

int locs_run::status(void)
{
    char lsm_key_max[KEY_SIZE];
    char lsm_key_min[KEY_SIZE];
    memset(lsm_key_max, 0, KEY_SIZE);
    memset(lsm_key_min, 0xFF, KEY_SIZE);

    if(Size == 0 && memcmp(max_key,lsm_key_max,KEY_SIZE)  == 0 && memcmp(min_key,lsm_key_min,KEY_SIZE)==0 && chunk_pointers.size()==0)
        return FULL_EMPTY;
    else
        return NOT_EMPTY;

}

void get_log_page_completion(void *arg, const struct spdk_nvme_cpl *completion)
{
    if (spdk_nvme_cpl_is_error(completion)) 
	{
        printf("OCSSD vector write failed with status 0x%x\n", completion->status.sct);
    }
	outstanding_commands--;
	
}

static void
print_ocssd_chunk_info(struct spdk_ocssd_chunk_information_entry *chk_info, int chk_num)
{
	int i;
	char *cs_str, *ct_str;

	printf("OCSSD Chunk Info Glance\n");
	printf("======================\n");

	

		printf("------------\n");
		printf("Chunk index:                    %d\n", chk_num);
		printf("Chunk state:                    %s(0x%x)\n", cs_str, *(uint8_t *) & (chk_info[chk_num].cs));
		printf("Chunk type (write mode):        %s\n", ct_str);
		printf("Chunk type (size_deviate):      %s\n", chk_info[chk_num].ct.size_deviate ? "Yes" : "No");
		printf("Wear-level Index:               %d\n", chk_info[chk_num].wli);
		printf("Starting LBA:                   %" PRIu64 "\n", chk_info[chk_num].slba);
		printf("Number of blocks in chunk:      %" PRIu64 "\n", chk_info[chk_num].cnlb);
		printf("Write Pointer:                  %" PRIu64 "\n", chk_info[chk_num].wp);
	
}


void locs_run::chunk_reset()
{
    
	int nsid = spdk_nvme_ns_get_id(ns);
	uint32_t num_entry = 256;
	uint32_t xfer_size = spdk_nvme_ns_get_max_io_xfer_size(ns);
	uint32_t buf_size = 0;
	uint64_t buf_offset = 0;
	outstanding_commands = 0;

	assert(num_entry != 0);

	spdk_ocssd_chunk_information_entry * chunks = (spdk_ocssd_chunk_information_entry *)spdk_dma_malloc(num_entry*sizeof(spdk_ocssd_chunk_information_entry), 0x40, NULL);
	assert(chunks != NULL);

	buf_size = num_entry * sizeof(struct spdk_ocssd_chunk_information_entry);
	while (buf_size > 0) 
    {
		xfer_size = min(buf_size, xfer_size);
		if (spdk_nvme_ctrlr_cmd_get_log_page(ctrlr, SPDK_OCSSD_LOG_CHUNK_INFO,nsid, (void *) (chunks + buf_offset),xfer_size, buf_offset, get_log_page_completion, NULL) == 0) 
        {
			outstanding_commands++;
		} 
        else 
        {
			printf("get_ocssd_chunk_info_log_page() failed\n");
			return ;
		}
		buf_size -= xfer_size;
		buf_offset += xfer_size;
	}


	while (outstanding_commands) 
    {
		spdk_nvme_ctrlr_process_admin_completions(ctrlr);
	}

    int err = 0;

    for (auto pointer: chunk_pointers)
    {
        err = insert_erase_queue(pointer, &chunks[pointer]);
        if(err !=0)
        {
            printf("Erase failed!");
        }
    }
    spdk_dma_free(chunks);
}

void locs_run::status_display(void)
{
    printf("Size:%lu,MaxSize:%lu,chunk_pointers size:%lu,,max_key:%lu,min_key:%lu",Size,MaxSize,chunk_pointers.size(), test(max_key), test(min_key));
}

bool locs_run::Isfull(void)
{
    return Size >= MaxSize;
}

bool locs_run::IsEmpty(void)
{
    return Size == 0;
}

uint64_t GetPagesize(void)
{
    return 4096 * 4;
}

uint64_t CalculatePageCapacity(size_t KVsize)
{
    return GetPagesize()/KVsize;
}