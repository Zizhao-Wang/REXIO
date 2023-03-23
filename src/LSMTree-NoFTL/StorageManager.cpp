#include "StorageManager.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
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

LSMTreeNoFTL::LSMTreeNoFTL(size_t BufferSize,int levelnum):
        buffer(BufferSize)
{
    for(uint32_t i=0;i<levelnum;i++)
    {
        NoFTLKVLevel temp(buffer.GetMaxSize(),i);
        Levels.emplace_back(temp);
    }
}

int LSMTreeNoFTL::FlushInto(vector<NoFTLKVLevel>::iterator current) 
{
    vector<NoFTLKVLevel>::iterator next;
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


    /**
    * Merge operation:
    * Merage all runs in the current level into the first run of next level.
    * There are three steps to merge datum of this level into the next level:
    * 1.Get the size of runs in the next level
    * 2.check if the size is greater than 0 
    * 3.Flush directly if the size equals 0, or merge all datum of next run and flush    
    **/
    if (next->IsFull()) 
    {
       /**
        * If the next level does not have space for the current level,
        * recursively merge the next level downwards to create some
        **/
        //printf("Next 1 run size:%lu Next 2 run size:%lu \n",next->Runs[0].GetNowSize(),next->Runs[1].GetNowSize());
        FlushInto(next);
        assert(next->IsEmpty());
        // /* Take over runs from current!*/
        // for (int i = 0;i<current->Runs.size();++i) 
        // {
        //     std::vector<PageType> pages  = current->Runs[i].GetPagePointers();
        //     if(next->Runs[0].SetPagePointers() == 0 && next->Runs[0].SetFencePointers(current->Runs[i].GetFencePointers()) == 0 &&next->Runs[0].SetMaxkey(current->Runs[i].GetMaxKey())==0)
        //     {
        //         current->Runs[i].Reset();
        //         printf("Size in take over:%lu\n",current->Runs[i].GetNowSize());
        //     }else{
        //         EMessageOutput("Page pointers merging failure in Level"+ Uint64toString(current->GetLevelNumber())+ "is trying to merging into Level"+ Uint64toString(next->GetLevelNumber())+"\n",110);
        //     }
        // }
        // assert(next->Runs[0].Isfull() == true);
    }
    
    for(int i=0;i<current->Runs.size();i++)
    {
        //printf("Run has %lu items in current level %ld\n",current->Runs[i].GetNowSize(),current->GetLevelNumber());
        if(current->Runs[i].GetNowSize() != 0)
        {
            mergecon.Insert(current->Runs[i].SingleRunRead());
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
    for(int i=0;i<current->Runs.size();i++)
    {
        current->Runs[i].Reset(); 
    }


    while(!mergecon.IsEmpty())
    {
        entry_t entry = mergecon.Contextpop1();
            // Remove deleted keys from the final level
        if ( entry.val != 0) 
        {
            next->PutValue(entry);
        }
    }
    
    return 0;
}

/*
 * "Put" operation can be divided into three step:
* 1.Put the value into buffer if buffer is not full
 * 2.Flush all datum witin buffer into level 1
 * 3.Put the value into empty buffer
 */

int LSMTreeNoFTL::PutValue(KEY_t key, VAL_t value) 
{
    /* Insert data into level 0 (buffer) */
    if(buffer.PutValue(key, value))  
    {
        return 1;
    }

    /* Step 2: Flush the buffer to level 0 */
    FlushInto(Levels.begin());  //check whether level 1 is full and flush it if level 1 is full 

    // Step 3
    std::vector<entry_t> bufferdata = buffer.GetEntries();

    if(Levels[0].IsEmpty())
    {
        for(auto& kv : bufferdata)
        {
            Levels[0].PutValue(kv);
        }
        // for(auto& run : Levels[0].Runs)
        // {
        //     printf("Run size: %ld from GetNowSize()\n",run.GetNowSize());
        // }
    }
    else
    {
        merge_context mergecon;
        mergecon.Insert(bufferdata);
        for(int i=0;i<Levels[0].Runs.size();i++)
        {
            if(Levels[0].Runs[i].GetNowSize()!= 0)
            {
                //printf("Run size: %ld from GetNowSize()\n",run.GetNowSize());
                mergecon.Insert(Levels[0].Runs[i].SingleRunRead());
                // printf("run size: %lu  ",run.SingleRunRead());
                Levels[0].Runs[i].Reset();
            }
        }
        //printf("Run size: %ld from GetNowSize()\n",run.GetNowSize());
        //std::vector<entry_t> values;
        //int i=0;
        while(!mergecon.IsEmpty())
        {
            entry_t entry = mergecon.Contextpop1();
            if (entry.val != 0) 
            {
                //values.emplace_back(entry);
                Levels[0].PutValue(entry);
            }
        }
        // for(int i=0;i<Levels[0].Runs.size();i++)
        // {
        //     printf("Run size: %ld from GetNowSize() in Level[0]\n",Levels[0].Runs[i].GetNowSize());
        // }
    }
    
    buffer.AllClear();
    assert(buffer.PutValue(key, value));

    return 0;
    
}

void LSMTreeNoFTL::UpdateValue(KEY_t key, VAL_t val)
{
    PutValue(key,val);
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

VAL_t* LSMTreeNoFTL::GetValue(KEY_t key) 
{
    
    VAL_t *buffer_val = new VAL_t;
    
    // /* Search buffe */
    
    buffer_val = buffer.GetValue(key);

    if (buffer_val != nullptr) 
    {
        return buffer_val;
    }

    delete buffer_val;
    
    // /* Search levels */
    VAL_t *level_val = new VAL_t;
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

void LSMTreeNoFTL::DeleteValue(KEY_t key) 
{
    PutValue(key, 0);
}
void LSMTreeNoFTL::display()
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

void NoFTLKVInit(void)
{

	clock_t startTime,endTime;                     // Definition of timestamp
    LSMTreeNoFTL noftlkv(256,7);                  // Initialize the LSMtree part of NoFTL-KV

	for(int i=0;i<100;++i)
    {
        memset(GPT[i],0,sizeof(GPT[i])); 
    }

    // printf("se:%d\n",GPT[0][105]);
    printf("\n ================ NoFTL-KV initialization information ================ \
            \n ---- Initialization successful! \
            \n ---- Global Page Table initialization successful! \n");

    /* workload a: insert only*/
    startTime = clock();
    for(SKey i=1;i<=524290;i++)
    {
        if(i%10000000==0||i==1000000)
        {
            endTime = clock();
            std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
        }
        noftlkv.PutValue(i,i);
    }
    printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    endTime = clock();
    std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    exit(0);
    //Lsmtree.display();
    //GPTDisplay();
    


    /* workload b: read only, all in it */
    startTime = clock();
     for(int i=1;i<=1000000;i++)
     {

        srand48(time(NULL));
        SKey k = 1+(rand()%4000000);

        if(noftlkv.GetValue(k)==nullptr)
        {
            printf("key:%lu Not Found!\n",k);
        }

        if(i==10000 || i%100000==0)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);   
        }
     }
    endTime = clock();
    printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     /* workload c: read only, 50% in it, 50% not in it */
     startTime = clock();
     
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        if(i%100<50)
        {
            SKey k = 1+(rand()%40000000);
            noftlkv.GetValue(k);
        }
        else
        {
            SKey k = 40000000+(rand()%40000000);
            noftlkv.GetValue(k);
        }
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
        }
    }
    printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    endTime = clock();
    std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    /* workload d: update heavy workload, 50% read, 50% update */
    startTime = clock();
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        if(i%2==0)
        {
            SKey k = 1+(rand()%40000000);
            noftlkv.GetValue(k);
        }
        else
        {
            SKey k = 1+(rand()%40000000);
            noftlkv.UpdateValue(k,k+1);
        }
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
        } 
    }
    printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    endTime = clock();
    std::cout << "Total Time of workload d: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     /* workload E: read mostly workload, 95% read, 5% update */
     startTime = clock();
     for(int i=1;i<=1000000;i++)
     {
        srand48(time(NULL));
        if(i%100<95)
        {
            SKey k = 1+(rand()%40000000);
            noftlkv.GetValue(k);
        }
        else
        {
            SKey k = 1+(rand()%40000000);
            noftlkv.UpdateValue(k,k+1);
        }
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
           printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
        } 
    }
    printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    endTime = clock();
    std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     /* workload F: read latest workload, 95% read, 5% insert */
    startTime = clock();
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        if(i%100<95)
        {
            SKey k = 1+(rand()%40000000);
            noftlkv.GetValue(k);
        }
        else
        {
            SKey k = 40000000+(rand()%40000000);
            noftlkv.PutValue(k,k+1);
        }
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n"; 
            printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
        } 
    }
    printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    endTime = clock();
    std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    /* workload G: delete workload, 100% delete*/
    startTime = clock();
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        SKey k = 1+(rand()%40000000);
        noftlkv.DeleteValue(k);
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";  
            printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
        }  
    }
    printf("Read count:%d Write count:%u Erase Count:%d \n",reads,writes,erases);
    endTime = clock();
    std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

	
}