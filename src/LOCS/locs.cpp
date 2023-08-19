#include "locs.h"

#include <map>
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include <spdk/env.h>  //  other project's .h files SPDK's .h files 
#include <spdk/log.h>
#include <spdk/nvme.h>
#include <spdk/nvme_ocssd.h>
#include <spdk/nvme_ocssd_spec.h>

#include "io_scheduler.h"
#include "global_variables.h"
#include "../Debug/debug_micros.h"
#include "../Backend/SSDWrite/writer.h"
#include "../Backend/backend_variables.h"


/*
 * LSM Tree
 * @BufferSize
 * @depth
 * @fanout
 * @num_threads
 * @bf_bits_per_entry
 */


bool debug_flag = false;

LOCS::LOCS(size_t BufferSize,int levelnum):buffer(BufferSize)
{
    for(uint32_t i=0;i<levelnum;i++)
    {
        locs_level temp(buffer.GetMaxSize());
        Levels.emplace_back(temp);
    }
}

int LOCS::FlushInto(vector<locs_level>::iterator current) 
{

    vector<locs_level>::iterator next;
    merge_context mergecon;
    entry_t entry;

    AssertCondition(current >= Levels.begin());

    if (current->IsFull()) 
    {
        next = current + 1;
    }
    else
    {
        return 0;
    }

    if (next->IsFull()) 
    {
       /**
        * If the next level does not have space for the current level,
        * recursively merge the next level downwards to create some
        **/
        FlushInto(next);
        assert(next->IsEmpty());
    }

    printf("  === Level %ld  Debug Information in LOCS::FlushInto (Call ) ===\n", next->GetLevelNumber());

    /**
    * Merge operation:
    * Merage all runs in the current level into the first run of next level.
    * There are three steps to merge datum of this level into the next level:
    * 1.Get the size of runs in the next level
    * 2.check if the size is greater than 0 
    * 3.Flush directly if the size equals 0, or merge all datum of next run and flush    
    **/
    if(!next->IsFull() && !next->IsEmpty())
    {
        char deleted_val[VAL_SIZE];
        memset(deleted_val, 0, VAL_SIZE);
        for(int i=0;i<current->Runs.size();i++)
        {
            //printf("Run has %lu items in current level %ld\n",current->Runs[i].GetNowSize(),current->GetLevelNumber());
            if(current->Runs[i].GetNowSize() != 0)
            {
                mergecon.Insert(current->Runs[i].SingleRunRead());
                current->Runs[i].Reset(); 
            }
        }

        
        for(int i=0;i<next->Runs.size();i++)
        {
            //printf("Run has %lu items in next level %ld!\n",next->Runs[i].GetNowSize(),next->GetLevelNumber());
            if(next->Runs[i].GetNowSize() != 0)
            {
                mergecon.Insert(next->Runs[i].SingleRunRead());
                next->Runs[i].Reset();
            }
        }

#ifdef DELAYED_ERASE
        check_if_erase();
#endif

        while(!mergecon.IsEmpty())
        {
            entry_t entry = mergecon.Contextpop();
            uint64_t key_value = test(entry.key);
            if (memcmp(entry.val, deleted_val, VAL_SIZE)!=0) 
            {
                next->PutValue(entry);
            }
            if(next->IsFull())
            {
                FlushInto(next);
            }
        }

        printf("  === Merging Level %ld done! ===\n", next->GetLevelNumber());

        return 0;
    }

    

    /* Take over runs from current!*/
    std::vector<PageType> page_poniters;
    char maxkey[KEY_SIZE];
    char minkey[KEY_SIZE];
    memset(maxkey, 0, KEY_SIZE);
    memset(minkey, 0xFF, KEY_SIZE);
    uint64_t count_size = 0;
    static int call_count = 0;
    call_count++;

    for (int i = 0;i<current->Runs.size();++i) 
    {
        std::vector<PageType> temp = current->Runs[i].GetPagePointers();
        page_poniters.insert(page_poniters.end(),temp.begin(),temp.end());
        if(memcmp(current->Runs[i].get_max_key(),maxkey, KEY_SIZE) > 0)
        {
            memcpy(maxkey, current->Runs[i].get_max_key(), KEY_SIZE);
        }
        
        if(memcmp(current->Runs[i].get_min_key(),minkey, KEY_SIZE) < 0)
        {
            memcpy(minkey, current->Runs[i].get_min_key(), KEY_SIZE);
        }
        
        count_size += current->Runs[i].GetNowSize();
        current->Runs[i].Reset();
    }

    if(next->Runs[0].status() == FULL_EMPTY)
    {
        next->Runs[0].set_chunk_pointers(page_poniters); 
        next->Runs[0].set_max_key(maxkey);
        next->Runs[0].set_min_key(minkey);
        next->Runs[0].set_current_size(count_size);
    }
    else
    {
        next->Runs[0].status_display();
        EMessageOutput("Page pointers merging failure in Level "+ Uint64toString(current->GetLevelNumber())+ "is trying to merging into Level"+ Uint64toString(next->GetLevelNumber())+"\n",110);
    }

#ifdef DEBUG
    printf("  === Level %ld: Debug Information in LOCS::FlushInto (Call %d) ===\n", next->GetLevelNumber(), call_count);
    printf("    Merging Page Pointers from Level %ld into Level %ld\n", current->GetLevelNumber(), next->GetLevelNumber());
    printf("    Min Key: %lu", test(minkey));
    printf("    Max Key: %ld\n", test(maxkey));
    printf("    Page Pointers (%lu): ", page_poniters.size());

    for (int i = 0; i < page_poniters.size(); i++) 
    {
        printf(" %lu ", page_poniters[i]);
    }

    printf("\n");
    printf("  === Flush Into Debug Information End (Call %d) ===\n", call_count);
#endif

    return 0;
}

/*
 * "Put" operation can be divided into three step:
 * 1.Put the value into buffer if buffer is not full
 * 2.Flush all datum witin buffer into level 1
 * 3.Put the value into empty buffer
 */

int LOCS::PutValue(const char* key, const char* value) 
{

    writes_ram++;
    /* Insert data into level 0 (buffer) */
    if(buffer.PutValue(key, value))  
    {
        return 1;
    }

#ifdef DEBUG
    printf("=== Level Debug Information Begin: ===\n");
#endif

    /* Step 2: Flush the buffer to level 0 */
    FlushInto(Levels.begin());  //check whether level 1 is full and flush it if level 1 is full 

    // Step 3
    std::vector<entry_t> bufferdata = buffer.GetEntries();

#ifdef TIME_RECORDING_ON
    auto start_time = std::chrono::high_resolution_clock::now();
#endif
   
    if(Levels[0].IsEmpty())
    {
        for(auto& kv : bufferdata)
        {
            Levels[0].PutValue(kv);
        }
    }
    else
    {
        merge_context mergecon;
        char deleted_val[KEY_SIZE];
        memset(deleted_val, 0, KEY_SIZE);
        // printf("start!\n");
        for(int i=0;i<Levels[0].Runs.size();i++)
        {
            if(Levels[0].Runs[i].GetNowSize()!= 0)
            {
                // printf("Run size: %ld from GetNowSize()\n",Levels[0].Runs[i].GetNowSize());
                mergecon.Insert(Levels[0].Runs[i].SingleRunRead());
                Levels[0].Runs[i].Reset(); 
            }
        }
        mergecon.Insert(bufferdata);   
        // printf("Run size: %ld from bufferdata\n",bufferdata.size());
        int i=0;
        while(!mergecon.IsEmpty())
        {
            entry_t entry = mergecon.Contextpop();
            uint64_t key_value = test(entry.key);
            // printf("key_value: %lu\n", key_value);
            if (memcmp(entry.val, deleted_val,KEY_SIZE)!=0) 
            {
                Levels[0].PutValue(entry);
                i++;
            }
            if(Levels[0].IsFull())
            {
                FlushInto(Levels.begin());
            }
        }
    }

#ifdef DELAYED_ERASE
    check_if_erase();
#endif

    buffer.AllClear();
    assert(buffer.PutValue(key, value));
    
#ifdef TIME_RECORDING_ON
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Total I/O time for clear data: " << duration.count() << "ms\n";
#endif

#ifdef DEBUG
    for (auto level_it = Levels.begin(); level_it != Levels.end(); ++level_it) 
    {
        if (level_it->IsEmpty()) 
        {
            continue;
        }
        printf("  === Level %ld Information ===\n", level_it->GetLevelNumber());
        for (size_t i = 0; i < level_it->Runs.size(); i++) 
        {
            std::vector<PageType> page_pointers = level_it->Runs[i].GetPagePointers();
            size_t dataSize = level_it->Runs[i].GetNowSize();

            if (dataSize == 0) 
            {
                printf("  --- Run %zu: Data Size: 0 ---\n", i);
                continue;
            }

            printf("  --- Run %zu ---\n", i);
            printf("    Data Size: %lu\n", dataSize);
            printf("    Min Key: %lu  ", test(level_it->Runs[i].get_min_key()));
            printf(" Max Key: %lu\n", test(level_it->Runs[i].get_max_key()));
            printf("    Page Pointers (%lu): ", page_pointers.size());
            for (size_t j = 0; j < page_pointers.size(); j++) 
            {
                printf("%lu ", page_pointers[j]);
            }
            printf("\n\n");
        }
    }
    printf("=== Level Debug Information End ===\n\n");
#endif
    return 0;
}



void LOCS::UpdateValue(const char* key, const char* value)
{
    PutValue(key,value);
}

// Run * LSMTree::get_run(int index) 
// {
//     for (const auto& level : Levels) 
//     {
//         if (index < level.runs.size()) 
//         {
//             return (Run *) &level.runs[index];
//         }
//         else 
//         {
//              index -= level.runs.size();
//         }
//     }

//     return nullptr;
// };

const char* LOCS::GetValue(const char* key) 
{
    
    const char *buffer_val = nullptr;
    
    // /* Search buffe */
    
    buffer_val = buffer.GetValue(key);

    if (buffer_val != nullptr) 
    {
        return buffer_val;
    }

    delete buffer_val;
    
    // /* Search levels */
    const char *level_val = nullptr;
    for(int i=0;i<Levels.size();++i)
    {
        //printf("Search in Level %d\n",i);
        level_val = Levels[i].GetValue(key);
        if(level_val!=nullptr)
        {
            return level_val;
        }
        //printf("==============\n");
    }
    
    delete level_val;
    return nullptr; 
     
//         int current_run;
//         Run *run;
//         
//          counter = 0;
//          latest_run = -1;
//         current_run = counter++;

//         if (latest_run >= 0 || (run = get_run(current_run)) == nullptr) 
//         {
//             // Stop search if we discovered a key in another run, or
//             // if there are no more runs to search
//             return;
//         } 
//         else if ((current_val = run->get(key)) == nullptr) 
//         {
//             // Couldn't find the key in the current run, so we need
//             // to keep searching.
//             search();
//         } else 
//         {
//             // Update val if the run is more recent than the
//             // last, then stop searching since there's no need
//             // to search later runs.
//             lock.lock();

//             if (latest_run < 0 || current_run < latest_run) 
//             {
//                 latest_run = current_run;
//                 latest_val = *current_val;
//             }

//             lock.unlock();
//             delete current_val;
//         }
//     };

//     if (latest_run >= 0 && latest_val != VAL_TOMBSTONE) cout << latest_val;
//     cout << endl;
}

// void LSMTree::GetRange(KEY_t start, KEY_t end) 
// {
//     vector<entry_t> *buffer_range;
//     map<int, vector<entry_t> *> ranges;
//     SpinLock lock;
//     atomic<int> counter;
//     merge_context merge_ctx;
//     entry_t entry;

//     if (end <= start) {
//         cout << endl;
//         return;
//     } else {
//         // Convert to inclusive bound
//         end -= 1;
//     }

//     /*
//      * Search buffer
//      */

//     ranges.insert({0, buffer.range(start, end)});

//     /*
//      * Search runs
//      */

//     counter = 0;

//     worker_task search = [&] {
//         int current_run;
//         Run *run;

//         current_run = counter++;

//         if ((run = get_run(current_run)) != nullptr) {
//             lock.lock();
//             ranges.insert({current_run + 1, run->range(start, end)});
//             lock.unlock();

//             // Potentially more runs to search.
//             search();
//         }
//     };

//     worker_pool.launch(search);
//     worker_pool.wait_all();

//     /*
//      * Merge ranges and print keys
//      */

//     for (const auto& kv : ranges) {
//         merge_ctx.add(kv.second->data(), kv.second->size());
//     }

//     while (!merge_ctx.done()) {
//         entry = merge_ctx.next();
//         if (entry.val != VAL_TOMBSTONE) {
//             cout << entry.key << ":" << entry.val;
//             if (!merge_ctx.done()) cout << " ";
//         }
//     }

//     cout << endl;

//     /*
//      * Cleanup subrange vectors
//      */

//     for (auto& range : ranges) {
//         delete range.second;
//     }
// }

void LOCS::DeleteValue(const char* key) 
{
    resets++;
    char deleted_val[KEY_SIZE];
    memset(deleted_val, 0, KEY_SIZE);
    PutValue(key, deleted_val);
}

void LOCS::display()
{
    buffer.display();
}

// void LSMTree::load(string file_path) 
// {
//     ifstream stream;
//     entry_t entry;

//     stream.open(file_path, ifstream::binary);

//     if (stream.is_open()) {
//         while (stream >> entry) {
//             put(entry.key, entry.val);
//         }
//     } else {
//         die("Could not locate file '" + file_path + "'.");
//     }
// }



void locs_init(void)
{

	clock_t startTime,endTime;          // Definition of timestamp
    LOCS locs(2,7);                  // Initialize the memory part of LOCS

    /* workload a: insert only*/
    uint64_t written_data_size = 100000000*16;
    uint64_t written_data_num = written_data_size /(KEY_SIZE+VAL_SIZE);
    uint64_t record_point = written_data_num / 10;

    startTime = clock();
    int error_bound = KEY_SIZE > 8 ? 8 : 0;
    char key_buffer[KEY_SIZE];
    char value_buffer[VAL_SIZE];
    // 4194305

    for(SKey i=1;i<=written_data_num;i++)
    {

        memset(key_buffer, 0, KEY_SIZE);
        memset(value_buffer, 0, VAL_SIZE);

        for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
        {
            key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((i >> (8 * j)) & 0xFF);
        }
        for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
        {
            value_buffer[KEY_SIZE - 1 - j] = static_cast<char>((i >> (8 * j)) & 0xFF);
        }

        locs.PutValue(key_buffer, value_buffer);

        if(i%record_point==0)
        {
            endTime = clock();
            printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
            std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            std::string experiment_name = "KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
            write_data( "/home/data/NOFTL_KV_Experiment.txt", experiment_name, "workload_A", i);

            io_resets = 0; 
        }
    }
    endTime = clock();
    std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC <<" End IO TIME: "<<time_record<<"  "<<time_record2 <<  "s\n";
    std::cout<<"Total IO Time3: "<< time_record3 <<"s\n";

    // /* workload b: read only, all in it */
    // startTime = clock();
    //  for(int i=1;i<=1;i++)
    //  {

    //     srand48(time(NULL));
    //     SKey k = 1+(rand()%100);

    //     memset(key_buffer, 0, KEY_SIZE);
    //     for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
    //     {
    //         key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
    //     }
    //     const char* value = locs.GetValue(key_buffer);
    //     if(value==nullptr)
    //     {
    //         printf("key:%lu Not Found!\n",k);
    //     }
    //     // printf("\n");
    //     if(i==10000 || i%100000==0)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
    //         // printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);   
    //     }
    //  }
    // endTime = clock();
    // // printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    // std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    //  /* workload c: read only, 50% in it, 50% not in it */
    //  startTime = clock();
     
    // for(int i=1;i<=1000000;i++)
    // {
    //     srand48(time(NULL));
    //     if(i%100<50)
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         locs.GetValue(k);
    //     }
    //     else
    //     {
    //         SKey k = 40000000+(rand()%40000000);
    //         locs.GetValue(k);
    //     }
    //     if(i%100000==0 || i==10000)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
    //         printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    //     }
    // }
    // printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    // endTime = clock();
    // std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    reads_io = 0;
    writes_io = 0;
    writes_ram = 0;
    resets = 0;
    io_resets = 0;
    uint64_t* workd = new uint64_t[written_data_num/2+10];
    string x;
    int index1=0;
    ifstream inFile;
    std::ostringstream filenameStream;
    filenameStream << "/home/data/update_" << KEY_SIZE << "_" << VAL_SIZE << ".txt";
    std::string filename = filenameStream.str();
    std::cout << "update Filename: " << filename << std::endl;
    inFile.open(filename.c_str());
    if (!inFile) 
    {
        std::cerr << "Unable to open file datafile.txt";
        exit(1);   // call system to stop
    }
    while (getline(inFile,x) && index1 < (written_data_num/2+10))     
    {
        workd[index1++] = atoi(x.c_str());
    }
    inFile.close();
    /* workload d: update heavy workload, 50% read, 50% update */
    startTime = clock();
    record_point = written_data_num/2/10;
    debug_flag = true;
    for(uint64_t i=1;i<=written_data_num/2;i++)
    {
        memset(key_buffer, 0, KEY_SIZE);
        memset(value_buffer,0, VAL_SIZE);
        SKey k = workd[i-1];
          
        for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
        {
            key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
        }
        for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
        {
            value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k+1 >> (8 * j)) & 0xFF);
        }
        locs.UpdateValue(key_buffer,value_buffer);
          
        if(i%record_point==0)
        {
            printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            // printBlockInformation();
            std::string experiment_name = "KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
            write_data( "/home/data/NOFTL_KV_Experiment.txt", experiment_name, "workload_D", i);
            io_resets = 0;      
        }
    }
    printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
    endTime = clock();
    std::cout << "Total Time of workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

    //  /* workload E: read mostly workload, 95% read, 5% update */
    //  startTime = clock();
    //  for(int i=1;i<=1000000;i++)
    //  {
    //     srand48(time(NULL));
    //     if(i%100<95)
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         locs.GetValue(k);
    //     }
    //     else
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         locs.UpdateValue(k,k+1);
    //     }
    //     if(i%100000==0 || i==10000)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
    //        printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    //     } 
    // }
    // printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    // endTime = clock();
    // std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    //  /* workload F: read latest workload, 95% read, 5% insert */
    // startTime = clock();
    // for(int i=1;i<=1000000;i++)
    // {
    //     srand48(time(NULL));
    //     if(i%100<95)
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         locs.GetValue(k);
    //     }
    //     else
    //     {
    //         SKey k = 40000000+(rand()%40000000);
    //         locs.PutValue(k,k+1);
    //     }
    //     if(i%100000==0 || i==10000)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n"; 
    //         printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    //     } 
    // }
    // printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    // endTime = clock();
    // std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


    reads_io = 0;
    writes_io = 0;
    writes_ram = 0;
    resets = 0;
    io_resets = 0;
    uint64_t* workg = new uint64_t[written_data_num+10];
    printf("wrriten_data_num:%lu wrritten_data_num:%lu\n",written_data_num,written_data_num/5+10);
    index1=0;
    clearBufferLog();
    filenameStream.str("");
    filenameStream.clear();
    filenameStream << "/home/data/delete_" << KEY_SIZE << "_" << VAL_SIZE << ".txt";
    filename = filenameStream.str();
    std::cout << "delete Filename: " << filename << std::endl;
    inFile.open(filename.c_str());
    if (!inFile) 
    {
        std::cerr << "Unable to open file datafile.txt"<<endl;
        exit(1);   // call system to stop
    }
    while (getline(inFile,x) && index1<(written_data_num+10))     
    {
        workg[index1++] = atoi(x.c_str());
    }
    printf("index1:%d\n",index1);
    inFile.close();
    /* workload G: delete workload, 100% delete*/
    startTime = clock();
    record_point = written_data_num/5/10;
    for(int i=1;i<=written_data_num/5;i++)
    {
        srand48(time(NULL));
        SKey k = workg[i-1];
        // printf("key:%lu\n",k);
        memset(key_buffer, 0, KEY_SIZE);
        for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
        {
            key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
        }
        locs.DeleteValue(key_buffer);
        if(i%record_point==0)
        {
            printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            // printBlockInformation();
            std::string experiment_name = "KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
            write_data( "/home/data/NOFTL_KV_Experiment.txt", experiment_name, "workload_G", i);
            io_resets = 0;     
        }  
    }
    printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
    endTime = clock();
    std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
    delete [] workg;
    delete [] workd;
}

void locs_close()
{

    // free I/O queues
    for(int i=0; i<geometry.num_grp;i++)
    {
        spdk_nvme_ctrlr_free_io_qpair(channels[i].qpair);
    }

    spdk_nvme_detach_async(ctrlr, &g_detach_ctx);

    spdk_dma_free(chunks);
}