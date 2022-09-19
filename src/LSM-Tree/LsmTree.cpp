#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include "LsmTree.h"

/*
 * LSM Tree
 * @BufferSize
 * @depth
 * @fanout
 * @num_threads
 * @bf_bits_per_entry
 */

LSMTree::LSMTree(int BufferSize, int NumThreads) :
        buffer(BufferSize),worker_pool(NumThreads){}

int LSMTree::FlushInto(vector<Level>::iterator current) 
{
    vector<Level>::iterator next;
    MergeContext mergecon;
    entry_t entry;

    AssertCondition(current >= Levels.begin());
    if (current->IsFull()) 
    {
        if(current+1 == Levels.end())
        {
            Level temp(buffer.GetMaxSize());
            Levels.emplace_back(temp);
        }
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
        FlushInto(next);
        AssertCondition(next->IsEmpty());

        long sizecount = 0;
        AssertCondition(next->Runs.front().GetNowSize() == 0);
        for (auto& run : current->Runs) 
        {
            if(next->Runs.front().SetPagePointers(run.GetPagePointers()) == -1 && next->Runs.front().SetFencePointers(run.GetFencePointers()) == -1)
            {
                EMessageOutput("Page pointers merging failure in Level"+ Uint64toString(current->GetLevelNumber())+ "is trying to merging into Level"+ Uint64toString(next->GetLevelNumber())+"\n",110);
            }
            sizecount += run.GetNowSize();
        }
        AssertCondition(sizecount == next->Runs.front().GetNowSize());
        for (auto& run : current->Runs) 
        {
            run.Unbind();
        }
    }
    else
    {

        for (auto& run : current->Runs) 
        {
            if(run.GetNowSize() != 0)
            {
                mergecon.Insert(run.SingleRunRead(), run.GetNowSize());
            }
        }
        for(auto& run : next->Runs)
        {
            if(run.GetNowSize() != 0)
            {
                mergecon.Insert(run.SingleRunRead(), run.GetNowSize());
            }
        }

        std::vector<entry_t> values;
        while(!mergecon.IsEmpty())
        {
            entry_t entry = mergecon.Contextpop();
            // Remove deleted keys from the final level
            if ( entry.val != VAL_TOMBSTONE) 
            {
                values.emplace_back(entry);
            }
        }
        next->PutEntries(values);
        for (auto& run : current->Runs) 
        {
            run.Unbind();
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

int LSMTree::PutValue(KEY_t key, VAL_t value) 
{
    //Step 1
    if(buffer.PutValue(key, value))  
    {
        return 1;
    }

    /* Step 2: Flush the buffer to level 0 */
    if(Levels.size()==0)
    {
        Level temp(buffer.GetMaxSize());
        Levels.emplace_back(temp);
    }
    FlushInto(Levels.begin());  //check whether level 1 is full and flush it if level 1 is full 


    // Step 3
    std::vector<entry_t> bufferdata = buffer.GetEntries();
    if(Levels[0].IsEmpty())
    {
        for(auto& kv : bufferdata)
        {
            Levels[0].PutValue(kv);
        }
    }
    else
    {
        MergeContext mergecon;
        mergecon.Insert(bufferdata);
        for(auto& run : Levels[0].Runs)
        {
            if(run.GetNowSize()!= 0)
            {
                mergecon.Insert(run.SingleRunRead(), run.GetNowSize());
            }
        }
        std::vector<entry_t> values;
        while(!mergecon.IsEmpty())
        {
            entry_t entry = mergecon.Contextpop();
            if (entry.val != VAL_TOMBSTONE) 
            {
                values.emplace_back(entry);
            }
        }
        Levels[0].PutEntries(values);
    }
    
    buffer.AllClear();
    AssertCondition(buffer.PutValue(key, value));

    return 0;
    
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
//             index -= level.runs.size();
//         }
//     }

//     return nullptr;
// };

// void LSMTree::GetValue(KEY_t key) {
//     VAL_t *buffer_val;
//     VAL_t latest_val;
//     int latest_run;
//     SpinLock lock;
//     atomic<int> counter;

//     /*
//      * Search buffer
//      */

//     buffer_val = buffer.get(key);

//     if (buffer_val != nullptr) {
//         if (*buffer_val != VAL_TOMBSTONE) cout << *buffer_val;
//         cout << endl;
//         delete buffer_val;
//         return;
//     }

//     /*
//      * Search runs
//      */

//     counter = 0;
//     latest_run = -1;

//     worker_task search = [&] {
//         int current_run;
//         Run *run;
//         VAL_t *current_val;

//         current_run = counter++;

//         if (latest_run >= 0 || (run = get_run(current_run)) == nullptr) {
//             // Stop search if we discovered a key in another run, or
//             // if there are no more runs to search
//             return;
//         } else if ((current_val = run->get(key)) == nullptr) {
//             // Couldn't find the key in the current run, so we need
//             // to keep searching.
//             search();
//         } else {
//             // Update val if the run is more recent than the
//             // last, then stop searching since there's no need
//             // to search later runs.
//             lock.lock();

//             if (latest_run < 0 || current_run < latest_run) {
//                 latest_run = current_run;
//                 latest_val = *current_val;
//             }

//             lock.unlock();
//             delete current_val;
//         }
//     };

//     worker_pool.launch(search);
//     worker_pool.wait_all();

//     if (latest_run >= 0 && latest_val != VAL_TOMBSTONE) cout << latest_val;
//     cout << endl;
// }

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
    PutValue(key, VAL_TOMBSTONE);
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
    LSMTree Lsmtree(64,5);                  // Initialize a LSM-tree structure

    /* Write datum */
    startTime = clock();
    for(uint64_t i=1;i<=1000000;i++)
    {
      if(i>=10000 && i%10000 ==0)
      {
        printf("Value:%lu \n",i);
        endTime = clock();
        std::cout << "Total Time of inserting: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
      } 
      uint64_t value = i;
      Lsmtree.PutValue(i,value);
    }
    endTime = clock();
    std::cout << "Total Time of inserting: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


    /* Read datum */
    startTime = clock();
    for(int i=1;i<=1000000;i++)
    {
      uint64_t value = i;
      // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of reading data: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    /* data update */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
        uint64_t value = i;
        // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of datum update: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
}