/**
 * @date   19/9/2022
 * @author zz.wang
 * @details
 *          The following code define "Level" class and "Run" class. 
 *          The first class storing datum from "buffer" or last "Level", every "level" has 2 or 2^i(i=2,3,4...) 
 *          "Runs" where the size equals size of buffer.
 *          Every Level has three parameters:
 *          LevelNumber: Number of a se4pecific Level.
 *          MaxRus: Maximum of Runs in a specific Level, it equals "2^LevelNumber(LevelNumber= 1,2,3,4...)" 
 *          MaxRunSize: Size of Run, it equals buffer size(In this OC-SSD mode(FEMU), the default value is 64(that means every Run can store 1MB datum)). 
 * IDENTIFICATION:
 *          src/LSM-Tree/UtilityDefine/run.h
 **/
#ifndef EXPERIMENT1_RUN_H
#define EXPERIMENT1_RUN_H

#include <vector>
#include <queue>
#include <map>
#include <queue>
#include <mutex>
#include <pthread.h>
#include <semaphore.h>
#include "../Auxizilary/pre_definition.h"
#include "../Auxizilary/SysOutput.h"
#include "../Debug/debug_micros.h"


class locs_run 
{
private:
    uint64_t Size, MaxSize, max_io_size, io_count, max_entries_per_vector,current_vector_index;
    uint64_t thread_num;
    char max_key[KEY_SIZE];
    char min_key[KEY_SIZE];
    std::vector<uint64_t> chunk_pointers;
    std::vector<std::vector<entry_t>> Rundata; 
    entry_t **data;
    std::map<size_t, uint64_t> results;
    std::vector<char*> FencePointers;
   
public:
    locs_run(uint64_t);
    uint64_t  RunDataWrite(size_t index);
    void PointersDisplay();
    void PutValue(entry_t entry);
    std::vector<entry_t> RunValuesRead(uint64_t PageNum);
    std::vector<entry_t> SingleRunRead();
    const char * GetValue(const char* key);
    std::vector<entry_t> * GetRange(KEY_t, KEY_t);
    std::vector<uint64_t> GetPagePointers(void);
    std::vector<char*> GetFencePointers(void);
    const char * get_max_key(void);
    const char * get_min_key(void);
    int set_chunk_pointers(std::vector<uint64_t>);
    int SetFencePointers(std::vector<char*>);
    void Reset();
    void set_max_key(const char* key);
    void set_min_key(const char* key);
    void set_current_size(uint64_t);
    void Unbind();
    int DataClear(std::vector<entry_t> );
    unsigned long GetNowSize();
    void chunk_reset();
    int status(void);
    void status_display(void);
    bool Isfull(void);
    bool IsEmpty(void);
};

struct thread_params
{
    locs_run* object;
    size_t index;
    uint64_t write_count;
};

class ThreadPool 
{
public:
    std::vector<pthread_t> threads;


private:
    std::vector<std::queue<std::pair<void* (*)(void*), void*>>> tasks;
    std::vector<sem_t> semaphores;
    sem_t tasks_completed_semaphore[2];
    int total_tasks = 0;

    size_t get_thread_id() const 
    {
        pthread_t current_thread = pthread_self();
        for (size_t i = 0; i < threads.size(); ++i)
        {
            if(pthread_equal(current_thread, threads[i]))
            {
                return i;
            }
        }
        return 0; 
    }

    static void* thread_main(void* arg) 
    {

        ThreadPool* pool = static_cast<ThreadPool*>(arg);
        size_t thread_id = pool->get_thread_id();
        
        while (true) 
        {
            // printf("ThreadPool: Thread %zu waiting for task\n", thread_id);
            sem_wait(&pool->semaphores[thread_id]);

            if (pool->tasks[thread_id].empty()) 
            {
                printf("ThreadPool: Thread %zu no task found, continue waiting\n", thread_id);
                continue;
            }

            auto task = pool->tasks[thread_id].front();
            pool->tasks[thread_id].pop();

            if (task.first == &ThreadPool::exit_thread) 
            {
                printf("ThreadPool: Thread %zu received exit signal\n", thread_id);
                break;
            }
        
            // printf("ThreadPool: Thread %zu executing task\n", thread_id);
            task.first(task.second);
            sem_post(&pool->tasks_completed_semaphore[thread_id]);
        
            // printf("ThreadPool: Thread %zu completed task, notifying main thread\n", thread_id);

        }

        return nullptr;
    }

public:
    ThreadPool(size_t num_threads)
    {
        
        // Resize vectors
        tasks.resize(num_threads);
        semaphores.resize(num_threads);
        
        // Initialize semaphores and start threads
        for (size_t i = 0; i < num_threads; i++) 
        {
            sem_init(&semaphores[i], 0, 0);
            pthread_t thread;
            pthread_create(&thread, nullptr, thread_main, this);
            threads.push_back(thread);
            sem_init(&tasks_completed_semaphore[i], 0, 0);
        }
    }

    ~ThreadPool() 
    {
        printf("ThreadPool: Waiting for tasks to complete\n");
        for (size_t i = 0; i < threads.size(); i++) 
        {
            // Tell thread to exit
            add_task(i, &ThreadPool::exit_thread, nullptr);
        }
        for (size_t i = 0; i < threads.size(); i++) 
        {
            // Wait for thread to exit
            pthread_join(threads[i], nullptr);
        }
        for (auto& semaphore : semaphores) 
        {
            sem_destroy(&semaphore);
        }
        for(auto& tasks_completed : tasks_completed_semaphore)
            sem_destroy(&tasks_completed);
    }

    void add_task(size_t thread_id, void* (*function)(void*), void* arg) 
    {
        // printf("ThreadPool: Adding task to thread %zu\n", thread_id);
        tasks[thread_id].push(std::make_pair(function, arg));
        sem_post(&semaphores[thread_id]);
        total_tasks++;
    }

    // Function that does nothing and is used to signal threads to exit
    static void* exit_thread(void* arg) 
    {
        return nullptr;
    }

    bool has_pending_tasks() const 
    {
        for (size_t i = 0; i < tasks.size(); i++) 
        {
            if (!tasks[i].empty()) 
            {
                return true;
            }
        }
        return false;
    }

    void wait_for_all_tasks() 
    {
        for (int i = 0; i < 2; ++i) 
        {
            sem_wait(&tasks_completed_semaphore[i]);
        }
    }

   
};

extern ThreadPool pool;

#endif