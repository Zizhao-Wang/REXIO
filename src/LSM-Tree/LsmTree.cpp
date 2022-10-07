#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include "LsmTree.h"

uint32_t LSMTreeReadPhysicalPage = 0;
uint32_t LSMTreeWritePhysicalPage = 0;
uint32_t LSMTreeErasehysicalPage = 0;


/*
 * LSM Tree
 * @BufferSize
 * @depth
 * @fanout
 * @num_threads
 * @bf_bits_per_entry
 */

LSMTree::LSMTree(size_t BufferSize,int levelnum) :
        buffer(BufferSize)
{
    for(int i=0;i<levelnum;i++)
    {
        Level temp(buffer.GetMaxSize());
        Levels.emplace_back(temp);
    }

}

int LSMTree::FlushInto(vector<Level>::iterator current) 
{
    vector<Level>::iterator next;
    MergeContext mergecon;
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
        printf("Run has test it\n");
        FlushInto(next);
        AssertCondition(next->IsEmpty());

        long sizecount = 0;

        /* Take over runs from current!*/
        for (int i = 0;i<current->Runs.size();++i) 
        {
            if(next->Runs.front().SetPagePointers(run.GetPagePointers()) == -1 && next->Runs.front().SetFencePointers(run.GetFencePointers()) == -1)
            {
                EMessageOutput("Page pointers merging failure in Level"+ Uint64toString(current->GetLevelNumber())+ "is trying to merging into Level"+ Uint64toString(next->GetLevelNumber())+"\n",110);
            }
            sizecount += run.GetNowSize();
        }
        AssertCondition(sizecount == next->Runs.front().GetNowSize());
    }
    else
    {
        printf("Run has test it222\n");
        for(int i=0;i<current->Runs.size();i++)
        {
            printf("Run has %lu items in  current\n",current->Runs[i].GetNowSize());
            if(current->Runs[i].GetNowSize() != 0)
            {
                mergecon.Insert(current->Runs[i].SingleRunRead());
                printf("Run has %lu items in  current\n",current->Runs[i].GetNowSize());
            }
        }
        for(int i=0;i<next->Runs.size();i++)
        {
            if(next->Runs[i].GetNowSize() != 0)
            {
                mergecon.Insert(current->Runs[i].SingleRunRead());
                printf("Run has %lu items in next\n",current->Runs[i].GetNowSize());
                current->Runs[i].Reset();
            }
        }

        while(!mergecon.IsEmpty())
        {
            entry_t entry = mergecon.Contextpop();
            // Remove deleted keys from the final level
            if ( entry.val != VAL_MAX) 
            {
                next->PutValue(entry);
            }
        }
        for(int i=0;i<next->Runs.size();i++)
        {
            printf("Run size: %ld from GetNowSize() \n",next->Runs[i].GetNowSize());
        }
        //next->PutEntries(values);  
    }
    for(int i=0;i<current->Runs.size();i++)
    {
        current->Runs[i].Reset(); 
    }
    
    return 0;
}

/*
 * "Put" operation can be divided into three step:
* 1.Put the value into buffer if buffer is not full
 * 2.Flush all datum witin buffer into level 1
 * 3.Put the value into empty buffer
 */

int LSMTree::PutValue(KEY_t key, VAL_t value) 
{
    //Step 1
    if(buffer.PutValue(key, value))  
    {
        return 1;
    }
    // printf("Entries size:%lu\n",buffer.Entries.size());

    /* Step 2: Flush the buffer to level 0 */
    if(Levels.size()==0)
    {
        
        Level temp(buffer.GetMaxSize());
        Levels.emplace_back(temp);
    }
    
    FlushInto(Levels.begin());  //check whether level 1 is full and flush it if level 1 is full 
    // exit(0);

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
        //exit(0);
    }
    else
    {
        MergeContext mergecon;
        //printf("buffer size: %lu  ",bufferdata.size());
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
            entry_t entry = mergecon.Contextpop();
            if (entry.val != VAL_MAX) 
            {
                //values.emplace_back(entry);
                Levels[0].PutValue(entry);
            }
        }
        for(int i=0;i<Levels[0].Runs.size();i++)
        {
            printf("Run size: %ld from GetNowSize()\n",Levels[0].Runs[i].GetNowSize());
        }
    }
    
    buffer.AllClear();
    AssertCondition(buffer.PutValue(key, value));

    return 0;
    
}

void LSMTree::UpdateValue(KEY_t key, VAL_t val)
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

void LSMTree::GetValue(KEY_t key) 
{

    VAL_t *buffer_val = new VAL_t;
    VAL_t latest_val;
    int latest_run;
    SpinLock lock;
    atomic<int> counter;

    /* Search buffe */

    buffer_val = buffer.GetValue(key);

    if (buffer_val != nullptr) 
    {
        delete buffer_val;
        return ;
    }
    delete buffer_val;

    /* Search levels */

    VAL_t *level_val = new VAL_t;
    for(auto lev:Levels)
    {
        level_val = lev.GetValue(key);
        if(level_val!=nullptr)
        {
            return ;
        }
    }
    delete level_val;
    return ; 
     
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
//     MergeContext merge_ctx;
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

void LSMTree::DeleteValue(KEY_t key) 
{
    PutValue(key, VAL_MAX);
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



void LSMTreeInit()
{
    clock_t startTime,endTime;                        // Definition of timestamp
    LSMTree Lsmtree(256,7);                  // Initialize a LSM-tree structure

    /* workload a: insert only*/
    startTime = clock();
    for(SKey i=1;i<=1000000;i++)
    {
        if(i%10000000==0||i==1000000)
        {
            endTime = clock();
            std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
        }
        //printf("Insert %lu successful!\n",i);
        Lsmtree.PutValue(i,i);
    }
    printf("Read count:%d Write count:%u Erase Count:%d \n",LSMTreeReadPhysicalPage,LSMTreeWritePhysicalPage,LSMTreeErasehysicalPage);
    endTime = clock();
    std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


    // /* workload b: read only, all in it */
    // startTime = clock();
    // for(int i=1;i<=110;i++)
    // {
    //     srand48(time(NULL));
    //     SKey k = 1+(rand()%120480);
    //     Lsmtree.GetValue(k);
    //     if(i==10000 || i%100000==0)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
    //     }
    // }
    // endTime = clock();
    // printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
    // std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    //  /* workload c: read only, 50% in it, 50% not in it */
    //  startTime = clock();
     
    // for(int i=1;i<=1000000;i++)
    // {
    //     srand48(time(NULL));
    //     if(i%100<50)
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         Lsmtree.GetValue(k);
    //     }
    //     else
    //     {
    //         SKey k = 40000000+(rand()%40000000);
    //         Lsmtree.GetValue(k);
    //     }
    //     if(i%100000==0 || i==10000)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
    //     }
    // }
    // //printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
    // endTime = clock();
    // std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    // /* workload d: update heavy workload, 50% read, 50% update */
    // startTime = clock();
    // for(int i=1;i<=1000000;i++)
    // {
    //     srand48(time(NULL));
    //     if(i%2==0)
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         Lsmtree.GetValue(k);
    //     }
    //     else
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         Lsmtree.UpdateValue(k,k+1);
    //     }
    //     if(i%100000==0 || i==10000)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
    //     } 
    // }
    //  //printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
    // endTime = clock();
    // std::cout << "Total Time of workload d: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    //  /* workload E: read mostly workload, 95% read, 5% update */
    //  startTime = clock();
    //  for(int i=1;i<=1000000;i++)
    //  {
    //     srand48(time(NULL));
    //     if(i%100<95)
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         Lsmtree.GetValue(k);
    //     }
    //     else
    //     {
    //         SKey k = 1+(rand()%40000000);
    //         Lsmtree.UpdateValue(k,k+1);
    //     }
    //     if(i%100000==0 || i==10000)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
    //     } 
    // }
    //  //printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
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
    //         Lsmtree.GetValue(k);
    //     }
    //     else
    //     {
    //         SKey k = 40000000+(rand()%40000000);
    //         Lsmtree.PutValue(k,k+1);
    //     }
    //     if(i%100000==0 || i==10000)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
    //     } 
    // }
    // //printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
    // endTime = clock();
    // std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    // /* workload G: delete workload, 100% delete*/
    // startTime = clock();
    // for(int i=1;i<=1000000;i++)
    // {
    //     srand48(time(NULL));
    //     SKey k = 1+(rand()%40000000);
    //     Lsmtree.DeleteValue(k);
    //     if(i%100000==0 || i==10000)
    //     {
    //         endTime = clock();
    //         std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
    //     }  
    // }
    // //printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
    // endTime = clock();
    // std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
}