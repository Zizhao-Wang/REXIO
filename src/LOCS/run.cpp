#include "run.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <cstdlib>
#include <pthread.h>
#include <cstring>
#include <cmath>
#include "../Backend/IODisk/WriteDisk.h"
#include "../Backend/SSDWrite/writer.h"
#include "../Backend/SSDRead/reader.h"
#include "global_variables.h"
#include "io_scheduler.h"


locs_run::locs_run(unsigned long maxsize)
{
    this->MaxSize = maxsize;
    this->max_io_size = get_max_data_entries_per_io();
    memset(this->max_key, 0, KEY_SIZE);
    memset(this->min_key, 0xFF, KEY_SIZE);
    this->Size = 0;
    this->io_count = 0;

    this->max_entries_per_vector = (geometry.clba/64)*(( SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES* page_size) / sizeof(entry_t));
    // printf("max_entries_per_vector:%lu\n",max_entries_per_vector);
    this->current_vector_index = 0;
    this->thread_num = geometry.num_grp;

    size_t num_vectors = MaxSize /chunk_capacity;
    data = new entry_t*[num_vectors];
    for(int i = 0; i < num_vectors; ++i)
    {
        data[i] = new entry_t[max_entries_per_vector];
    }
    // printf("max size:%lu\n",MaxSize);
    // printf("max_entries_per_vector:%lu\n",max_entries_per_vector);
    // printf("num_vectors:%lu\n",num_vectors);

    
}

void locs_run::PointersDisplay()
{
    for(size_t i=0;i<chunk_pointers.size();i++)
    {
        printf("page pointer:%lu\n",chunk_pointers[i]);
    }
}

std::vector<entry_t> locs_run::SingleRunRead()
{
    if(Size < MaxSize)
    {
        std::vector<entry_t> datas;
        int count = 0;
        for (int i = 0; i < MaxSize /chunk_capacity; ++i) 
        {
            for (int j = 0; j < max_entries_per_vector; ++j) 
            {
                if (count < Size) 
                {
                    datas.push_back(data[i][j]);
                    count++;
                } 
                else 
                {
                    break;
                }
            }
            if (count >= Size) 
            {
                break;
            }
        }
        return datas;
    }
    else
    {
        AssertCondition(Size == MaxSize);
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

            uint64_t channel_id = chunk_pointers[i] / (geometry.num_chk * geometry.num_pu);
            channels[channel_id].chunk_type[chunk_pointers[i] % (geometry.num_chk * geometry.num_pu)] = DISUSED_CHUNK;
        }
        return entries1;
    }
}

uint64_t locs_run::RunDataWrite(size_t index)
{
    // printf("index:%lu\n",index);
    // printf("max_entries_per_vector:%lu\n",max_entries_per_vector);
    select_write_queue(data[index], max_entries_per_vector, OCSSD_WRITE);
    return 0;  
}

void* parallel_data_write(void* arg) 
{

    auto data = static_cast<thread_params*>(arg);
    locs_run* object = data->object;
    size_t index = data->index;

    auto start_time = std::chrono::high_resolution_clock::now();
    object->RunDataWrite(index);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    time_record2 += duration.count();

    return nullptr;
}


void locs_run::PutValue(entry_t entry) 
{

#ifdef TIME_RECORDING_ON
    auto start_time0 = std::chrono::high_resolution_clock::now();
#endif
    
    assert(Size < MaxSize);
    // printf("Size:%lu==================\n",Size);
    data[Size/max_entries_per_vector][Size%max_entries_per_vector] = entry;
    
    if (memcmp(entry.key, max_key, KEY_SIZE) > 0) 
    {
        memcpy(max_key, entry.key, KEY_SIZE);
    }

    if (memcmp(entry.key, min_key, KEY_SIZE) < 0) 
    {
        memcpy(min_key, entry.key, KEY_SIZE);
    }

    Size++;
    
    if(Size % max_io_size==0 && Size != 0)
    {
        char fence_key [KEY_SIZE];
        memcpy(fence_key, entry.key, KEY_SIZE);
        FencePointers.emplace_back(fence_key);
    }

#ifdef TIME_RECORDING_ON
    auto end_time0 = std::chrono::high_resolution_clock::now();
    auto duration0 = std::chrono::duration_cast<std::chrono::milliseconds>(end_time0 - start_time0);
    time_record3 += duration0.count();
    printf("PutValue time:%lu\n",duration0.count());
#endif
    


#ifdef MULTI_THREAD_IO

#ifdef TIME_RECORDING_ON
    auto start_time = std::chrono::high_resolution_clock::now();
#endif
    
    if (Size == MaxSize) 
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        size_t max_concurrent_writes = geometry.num_grp;
        size_t total_vectors = MaxSize /chunk_capacity;
        for (size_t start = 0; start < total_vectors; start += max_concurrent_writes) 
        {
            size_t end = std::min(start + max_concurrent_writes, total_vectors);
            
            // threads create
            for (size_t i = start; i < end; i++) 
            {
                thread_params* args = new thread_params{this, i, UINT64_MAX};
                pool.add_task(i % geometry.num_grp, parallel_data_write, args);
            }

            // threads join
            // printf("waitting for join!\n");
            pool.wait_for_all_tasks();
            // printf("join finished!\n");

            for (size_t i = 0; i < geometry.num_grp; i++) 
            {
                chunk_pointers.emplace_back(temp_pointers[pool.threads[i]]);
            }
        }
    }

#ifdef TIME_RECORDING_ON
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    time_record += duration.count();
#endif
    
    
#elif defined(SINGLE_THREAD_IO)

    if (Size == MaxSize) 
    {
        size_t total_vectors = MaxSize / chunk_capacity;

        for (size_t i = 0; i < total_vectors; i++) 
        {
            RunDataWrite(i);
            chunk_pointers.emplace_back(temp_pointers);
            // printf("temp_pointers:%lu\n",temp_pointers);
        }
    }
    
#endif
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
    
    // // std::vector<uint64_t>::iterator it = lower_bound(FencePointers.begin(),FencePointers.end(),key);
    // size_t PageIndex = it->FencePointers.begin();

    // if(PageIndex < chunk_pointers.size() && chunk_pointers[PageIndex]!=UINT64_MAX)
    // {
    //     // reads = RunReadFromPage(PagePointers[PageIndex]);;
    //     // std::vector<entry_t>::iterator get;
    //     // get = find(reads.begin(),reads.end(),entry_t{key,0});
    //     // if(get!=reads.end())
    //     // {
    //     //     *value = (*get).val;
    //     //     return value; 
    //     // }
    // }
    // else
    // {
    //     // std::vector<entry_t>::iterator get;
    //     // get = find(Rundata.begin(),Rundata.end(),entry_t{key,0});
    //     // if(get!=reads.end())
    //     // {
    //     //     value = (*get).val;
    //     //     return value; 
    //     // }
    // }
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

    // for(size_t i=0;i<pointers.size();++i)
    // {
    //     FencePointers.emplace_back(pointers[i]);
    //     max_key = max(max_key,pointers[i]);
    // }

    // if(pointers.size()*CalculatePageCapacity(sizeof(entry_t))!=MaxSize/2 )
    // {
    //     return -1;
    // }

    return 0;
}

void locs_run::Reset()
{
    if(FencePointers.size()!=0)
    {
        FencePointers.clear();
    }

    Size = 0;

    if(chunk_pointers.size()!=0)
    {
        chunk_pointers.clear();
    }

    memset(max_key, 0, KEY_SIZE);
    memset(min_key, 0xFF, KEY_SIZE);
    io_count = 0;



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
			// printf("get_ocssd_chunk_info_log_page() failed\n");
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
        printf("pointer:%lu will be erased!\n",pointer);
        if(err !=0)
        {
            printf("Erase failed!");
            exit(-1);
        }
    }
    spdk_dma_free(chunks);
}

void locs_run::status_display(void)
{
    printf("Size:%lu,MaxSize:%lu,chunk_pointers size:%lu,,max_key:%lu,min_key:%lu",Size,MaxSize,chunk_pointers.size(),max_key,min_key);
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