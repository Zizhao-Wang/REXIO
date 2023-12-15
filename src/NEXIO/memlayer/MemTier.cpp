/**
 * @date    29/9/2022 
 * @author  zz.wang
 * @details
 *          If this is the program's first run, the program will create a new special hash table in memory.
 *          Otherwise, the process will read the hash table from the solid-state disk (SSD).
 *
 * IDENTIFICATION:
 *          src/TNCTree/MemoryTier/MemTier.cpp
 */
#include "MemTier.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <random>
#include <thread>
#include <iostream>
#include "utility/types.h"
#include "memlayer/syncstore.h"
#include "memlayer/asyncstore.h"
#include "include/sys_output.h"
#include "include/micros.h"
#include "include/init.h"

#define N 9999

thread_local std::mt19937 rng(std::random_device{}());



std::chrono::duration<double, std::milli> convert_and_find_bucket_time(0);
std::chrono::duration<double, std::milli> search_time(0);
std::chrono::duration<double, std::milli> insert_and_write_time(0);
std::chrono::duration<double, std::milli> split_time(0);



/**
 *  =================Node initialization module==================== 
 **/

bool LocalGeneration(GlobalHashNode *globalNode, int initialLocalDepth=0) 
{
#ifndef FastSkiplist
    TNCSkiplist *localHead = TskiplistCreate();
    if( initialLocalDepth != 0){
        localHead->depth = initialLocalDepth;
    }
    else{
        localHead->depth = Globaldepth+1;
    }

    globalNode->local = localHead;
    return true;
#endif

#ifdef FastSkiplist
    auto * localHead =(LocalHeadNode*) malloc(LOCAL_HEAD_SIZE);
    if(localHead!=nullptr || globalNode->local== nullptr)
    {
        if( initialLocalDepth != 0){
            localHead->depth = initialLocalDepth;
        }else{
            localHead->depth = Globaldepth+1;
        } 
        localHead->Nodenumber = 0;
        localHead->CurrentLevel = 1;

#ifdef NOT_SEPARATE_KV
        auto * localnodehead = Initialization();
#elif defined(NOT_SEPARATE_KV_variable)
        auto * localnodehead = Initialization();
#elif defined(SEPARATE_KV_FIXED_LOG)
        auto * localnodehead = Initialization();
#elif defined(SEPARATE_KV_VARIABLE_LOG)
        auto * localnodehead = Initialization();
#endif
        localHead->HashNode = localnodehead;
        globalNode->local = localHead;
        return true;
    }
    else
        return false;
#endif

   
}

/*
 * Methods belong to Special-Hash-Table.
 */
bool DoubleHashtable() 
{
    int nex = global.empty() ? 2 : (int)global.size();
    int initialLocalDepth = global.empty() ? 1 : 0;

    for (int i = 0; i < nex; ++i) 
    {
        auto *globalNode = (GlobalHashNode *)malloc(GLOBAL_HASH_SIZE);
        if (!LocalGeneration(globalNode, initialLocalDepth))
            return false;
        global.push_back(globalNode);
    }
    ++Globaldepth;

    return true;
}





// uint64_t big_endian2little_endian(const char *big_endian, size_t len)
// {
//     uint64_t result = 0;
//     size_t start_index = 0;
//     if (len > 8) {
//         start_index = len - 8;
//     }

//     for (size_t i = start_index; i < len; ++i) 
//     {
//         result |= (static_cast<uint64_t>(static_cast<unsigned char>(big_endian[i])) << (8 * (i - start_index)));
//     }
//     return result;
// }
// uint64_t big_endian2little_endian(const char *big_endian, size_t len)
// {
//     uint64_t result = 0;
//     size_t start_index = len > 8 ? len - 8 : 0;
//     size_t actual_len = len > 8 ? 8 : len;

//     for (size_t i = 0; i < (len - start_index); ++i)
//     {
//         result |= (static_cast<uint64_t>(static_cast<unsigned char>(big_endian[start_index + i])) << (8 * (actual_len-1-i)));
//     }

//     return result;
// }







/**
 *  =================Node Insertion module====================  
 **/

int RandomLevel()
{

#ifndef FastSkiplist
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    int level = 1;
    while (dist(rng) < P_FACTOR1 && level < MAX_LEVEL1) {
        level++;
    }
    return level;
#endif


#ifdef FastSkiplist
    int v = 1;
    srand ((unsigned int)(time (NULL)));
    while(rand() % (N + 1) / (float)(N + 1) < pro && v< MaxLevel )
    {
        v++;
    }
#endif

}

void  NodeSplit(size_t bucket_index)
{

#ifndef FastSkiplist
    TNCSkiplist *old_bucket = global[bucket_index]->local;
    if (old_bucket->depth < Globaldepth) 
    {
        
        int new_index = bucket_index | (1 << old_bucket->depth );
#ifdef TIOCS_DEBUG
        printf("new_index: %d\n", new_index);
#endif
        
        old_bucket->depth++;
    } 
    else 
    {
        if (!DoubleHashtable()) {
            printf("Failed to double the hash table!");
            exit(5735);
        }
        old_bucket->depth++;
    }

    
    uint64_t split_boundary = 1 << (old_bucket->depth - 1);
    TSkiplistNode *node = old_bucket->head->forward[0];
    TSkiplistNode *next;

    for(int i = 0; i < old_bucket->head->maxLevel; i++) 
    {
        old_bucket->head->forward[i] = NULL;
    }
    old_bucket->number = 0;
    old_bucket->level = 0;

    while (node) 
    {
        next = node->forward[0];

        uint64_t hash_key = big_endian2little_endian(node->key, KEY_SIZE);

        DEBUG_PRINT("Processing hash_key: %lu\n", hash_key);

        if (hash_key & split_boundary) 
        {
            int new_index = bucket_index | split_boundary;
            TNCSkiplist *new_bucket = global[new_index]->local;
            DEBUG_PRINT("Inserting hash_key %lu into new bucket %d\n", hash_key, new_index);
#ifdef NOT_SEPARATE_KV
            InsertNode(node->key, node->offset, node->flag, new_index);
#elif defined(NOT_SEPARATE_KV_variable)
            InsertNode(node->key, node->offset, node->flag, new_index);
#elif defined(SEPARATE_KV_FIXED_LOG)
            InsertNode(node->key, node->offset, node->flag,node->block, new_index);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
            InsertNode(node->key, node->offset, node->flag,node->block, new_index);
#endif
        } 
        else 
        {

            DEBUG_PRINT("Inserting hash_key %lu back into old bucket %zu\n", hash_key, bucket_index);

#ifdef NOT_SEPARATE_KV
            InsertNode(node->key, node->offset, node->flag, bucket_index);
#elif defined(NOT_SEPARATE_KV_variable)
            InsertNode(node->key, node->offset, node->flag, bucket_index);
#elif defined(SEPARATE_KV_FIXED_LOG)
            InsertNode(node->key, node->offset, node->flag,node->block, bucket_index);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
            InsertNode(node->key, node->offset, node->flag,node->block, bucket_index);
#endif

        }
        free(node->forward); 
        free(node); 
        node = next;
    }
#endif

#ifdef FastSkiplist
    LocalHeadNode *old_bucket = global[bucket_index]->local;
    if (old_bucket->depth < Globaldepth) 
    {
        
        int new_index = bucket_index | (1 << old_bucket->depth );
#ifdef TIOCS_DEBUG
        printf("new_index: %d\n", new_index);
#endif
        
        old_bucket->depth++;
    } 
    else 
    {
        if (!DoubleHashtable()) {
            printf("Failed to double the hash table!");
            exit(5735);
        }
        old_bucket->depth++;
    }

    
    uint64_t split_boundary = 1 << (old_bucket->depth - 1);
    LocalHashNode *node = old_bucket->HashNode->next[0];
    LocalHashNode *next;

    for(int i = 0; i < old_bucket->HashNode->maxLevel; i++) 
    {
        old_bucket->HashNode->next[i] = NULL;
    }
    old_bucket->Nodenumber = 0;
    old_bucket->CurrentLevel = 0;

    while (node) 
    {
        next = node->next[0];

        uint64_t hash_key = big_endian2little_endian(node->key, KEY_SIZE);

        DEBUG_PRINT("Processing hash_key: %lu\n", hash_key);

        if (hash_key & split_boundary) 
        {
            int new_index = bucket_index | split_boundary;
            LocalHeadNode *new_bucket = global[new_index]->local;
            DEBUG_PRINT("Inserting hash_key %lu into new bucket %d\n", hash_key, new_index);
#ifdef NOT_SEPARATE_KV
            InsertNode(node->key, node->offset, node->flag, new_index);
#elif defined(NOT_SEPARATE_KV_variable)
            InsertNode(node->key, node->offset, node->flag, new_index);
#elif defined(SEPARATE_KV_FIXED_LOG)
            InsertNode(node->key, node->offset, node->flag,node->block, new_index);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
            InsertNode(node->key, node->offset, node->flag,node->block, new_index);
#endif
        } 
        else 
        {

            DEBUG_PRINT("Inserting hash_key %lu back into old bucket %zu\n", hash_key, bucket_index);

#ifdef NOT_SEPARATE_KV
            InsertNode(node->key, node->offset, node->flag, bucket_index);
#elif defined(NOT_SEPARATE_KV_variable)
            InsertNode(node->key, node->offset, node->flag, bucket_index);
#elif defined(SEPARATE_KV_FIXED_LOG)
            InsertNode(node->key, node->offset, node->flag,node->block, bucket_index);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
            InsertNode(node->key, node->offset, node->flag,node->block, bucket_index);
#endif

        }
        free(node->next); 
        free(node); 
        node = next;
    }
#endif


    
}



#ifdef NOT_SEPARATE_KV
int InsertNode(const char* hashkey, uint64_t offset, uint8_t flag, int bucket_id)
#elif defined(NOT_SEPARATE_KV_variable)
int InsertNode(const char* hashkey, uint64_t offset, uint8_t flag, int bucket_id)
#elif defined(SEPARATE_KV_FIXED_LOG)
int InsertNode(const char* hashkey, uint64_t offset, uint8_t flag,uint64_t block, int bucket_id)
#elif defined(SEPARATE_KV_VARIABLE_LOG)
int InsertNode(const char* hashkey, uint64_t offset, uint8_t flag,uint64_t block, int bucket_id)
#endif
{

#ifndef FastSkiplist
    /*  Insert the hash value into special skip-list. */

    TNCSkiplist * Head = global[bucket_id]->local;
    TSkiplistNode * temp = Head->head;
    TSkiplistNode * update[MAX_LEVEL1];

    for(int i=Head->level; i>=0; i--)
    {
        while(temp->forward[i] && memcmp(temp->forward[i]->key,hashkey,KEY_SIZE)<0)
        {
            temp = temp->forward[i];
        }
        update[i] = temp;
    }

    
    int v=RandomLevel();
    assert(v <= MAX_LEVEL1);
    if(v > Head->level)
    {
        for(int i=Head->level; i<v; ++i )
        {
            update[i] = Head->head;
        }
        Head->level = v-1;
    }
    ++Head->number;
    DEBUG_PRINT("Inserting new node with hash_key: %.*s, offset: %u, flag: %u, level: %d\n",KEY_SIZE, hashkey, offset, flag, v);

#ifdef NOT_SEPARATE_KV
    TSkiplistNode* tsl = TskiplistNodeCreat(hashkey,offset,v);
#elif defined(NOT_SEPARATE_KV_variable)
    TSkiplistNode* tsl = TskiplistNodeCreat(hashkey,offset,v);
#elif defined(SEPARATE_KV_FIXED_LOG)
    TSkiplistNode* tsl = TskiplistNodeCreat(hashkey,offset,block,v);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    TSkiplistNode* tsl = TskiplistNodeCreat(hashkey,offset,block,v);
#endif

    tsl->flag = flag;  
    for(int i=0;i<v;++i)
    {
        tsl->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = tsl;
    }
#endif
    
#ifdef FastSkiplist 
    LocalHeadNode * head = global[bucket_id]->local;
    LocalHashNode * temp = head->HashNode;
    LocalHashNode * update[MaxLevel];

    int curLevel = head->CurrentLevel-1;

    for(int i=curLevel; i>=0; --i)
    {
        while(temp->next[i] && memcmp(temp->next[i]->key,hashkey,KEY_SIZE)< 0 )
        {
            temp = temp->next[i];
        }
        update[i] = temp;
    }


    int v=RandomLevel();
    assert(v <= MAX_LEVEL1);
    if(v > curLevel)
    {
        for(int i=curLevel+1; i<v; ++i )
        {
            update[i] = head->HashNode;
        }
       head->CurrentLevel = v+1;
    }
    ++head->Nodenumber;


#ifdef NOT_SEPARATE_KV
        temp = Initialization(hashkey,offset,v); 
#elif defined(NOT_SEPARATE_KV_variable)
        temp = Initialization(hashkey,offset,v);
#elif defined(SEPARATE_KV_FIXED_LOG)
        temp = Initialization(hashkey,offset,block,v);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
        temp = Initialization(hashkey,offset,block,v);
#endif

    temp->flag = flag;  
    for(int i=0;i<v;++i)
    {
        temp->next[i] = update[i]->next[i];
        update[i]->next[i] = temp;
    }
#endif


    return 0;
}

void PrintHashTable() 
{
    printf("==== Hash Table Content ====\n");
    for (int i = 0; i < (1 << Globaldepth); i++) 
    {
#ifndef FastSkiplist
        TNCSkiplist *bucket = global[i]->local;
#endif

#ifdef FastSkiplist
        LocalHeadNode *bucket = global[i]->local;
#endif
        printf("Bucket %d (depth: %d):\n", i, bucket->depth);
#ifndef FastSkiplist
        TSkiplistNode *node = bucket->head->forward[0];
#endif

#ifdef FastSkiplist
        LocalHashNode *node = bucket->HashNode->next[0];
#endif
        

        while (node) 
        {
            printf(" -Node with hash_key: %lu, offset: %lu, flag: %u, level: %d\n", big_endian2little_endian(node->key, KEY_SIZE), node->offset, node->flag, node->maxLevel);
#ifndef FastSkiplist               
           node = node->forward[0];
#endif
#ifdef FastSkiplist
            node = node->next[0];
#endif
        }
    }
    printf("=============================\n");
}









int InsertNode(const char* hashkey, const char* hashvalue)
{
#ifndef FastSkiplist 
    /*  Insert the hash value into special skip-list. */
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;
    uint64_t hashkey1 = big_endian2little_endian(hashkey,KEY_SIZE);
    int bucket_index = hashkey1 & (1<<Globaldepth)-1;
    TNCSkiplist * Head = global[bucket_index]->local;
    TSkiplistNode * temp = Head->head;
    TSkiplistNode * update[MAX_LEVEL1];
    end = std::chrono::high_resolution_clock::now();
    convert_and_find_bucket_time += (end - start);

    start = std::chrono::high_resolution_clock::now();
    if (Head->number >= max_bucket_size) 
    {
#ifdef TIOCS_DEBUG
        printf("Bucket %d is full, calling NodeSplit()\n", bucket_index);
        PrintHashTable();
#endif
        NodeSplit(bucket_index);
#ifdef TIOCS_DEBUG
        PrintHashTable();
#endif
    }
    end = std::chrono::high_resolution_clock::now();
    split_time += (end - start);

    start = std::chrono::high_resolution_clock::now();
    for(int i=Head->level; i>=0; i--)
    {
        while(memcmp(temp->forward[i]->key,hashkey,KEY_SIZE)<0)
        {
            temp = temp->forward[i];
        }
        update[i] = temp;
    }
    temp = temp->forward[0];
    end = std::chrono::high_resolution_clock::now();
    search_time += (end - start);

    start = std::chrono::high_resolution_clock::now();
    if(memcmp(temp->key, hashkey, KEY_SIZE)==0)
    {

            temp->flag =1;
            // write into disk

#ifdef NOT_SEPARATE_KV
            temp->offset = async_kv_write(hashkey,hashvalue); //printf("%u\n",temp->offset);
#elif defined(NOT_SEPARATE_KV_variable)
            temp->offset = async_kv_write(hashkey,hashvalue); //printf("%u\n",temp->offset);
#elif defined(SEPARATE_KV_FIXED_LOG)
            uint64_t block_id = 0;
            temp->offset = async_kv_separate_update(hashvalue,block_id);
            temp->block = block_id;
#elif defined(SEPARATE_KV_VARIABLE_LOG)
            uint64_t block_id = 0;
            temp->offset = 0 ;//  async_kv_separate_variable_update(hashvalue,block_id);
            temp->block = block_id;
#endif
            return 0;
    }
    else
    {
        int v=RandomLevel();
        if(v > Head->level)
        {
            for(int i=Head->level; i<v; ++i )
            {
                update[i] = Head->head;
            }
            Head->level = v-1;
        }
        //write into disk
#ifdef NOT_SEPARATE_KV
        uint64_t offset1 = async_write(hashkey,hashvalue); //printf("%u\n",temp->offset);
#elif defined(NOT_SEPARATE_KV_variable)
        uint64_t offset1 = async_kv_write(hashkey,hashvalue); //printf("%u\n",temp->offset);
#elif defined(SEPARATE_KV_FIXED_LOG)
        uint64_t block_id = 0;
        uint64_t offset1 = async_kv_separate_write(hashkey,hashvalue,block_id);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
        uint64_t block_id = 0;
        uint64_t offset1 = 0; //async_kv_separate_variable_write(hashkey,hashvalue,block_id); 
#endif

        ++Head->number;

#ifdef NOT_SEPARATE_KV
        temp = TskiplistNodeCreat(hashkey,offset1,v);
#elif defined(NOT_SEPARATE_KV_variable)
        temp = TskiplistNodeCreat(hashkey,offset1,v);
#elif defined(SEPARATE_KV_FIXED_LOG)
        temp = TskiplistNodeCreat(hashkey,offset1,block_id,v);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
        temp = TskiplistNodeCreat(hashkey,offset1,block_id,v);
#endif
        for(int i=0;i<v;++i)
        {
            temp->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = temp;
        }

        // if (hashkey1 >0) {
        //     printf("查询 hashkey1=2\n");
        //     printf("Head Level: %d\n", Head->level);
        //     TSkiplistNode *node = Head->head;
        //     while (node ) {
        //         printf("[%lu] -> ", big_endian2little_endian(node->key,KEY_SIZE));
        //         node = node->forward[0];
        //     }
        //     printf("(停止在此)\n");
        // }
    }
    end = std::chrono::high_resolution_clock::now();
    insert_and_write_time += (end - start);
#endif

#ifdef FastSkiplist 
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;

    uint64_t hashkey1 = big_endian2little_endian(hashkey,KEY_SIZE);
    int bucket_index = hashkey1 & (1<<Globaldepth)-1;
    LocalHeadNode * head = global[bucket_index]->local;
    LocalHashNode * temp = head->HashNode;
    LocalHashNode * update[MaxLevel];
    int curLevel = head->CurrentLevel-1;

    end = std::chrono::high_resolution_clock::now();
    convert_and_find_bucket_time += end - start;

    if (head->Nodenumber >= max_bucket_size) 
    {
#ifdef TIOCS_DEBUG
        printf("Bucket %d is full, calling NodeSplit()\n", bucket_index);
        PrintHashTable();
#endif
        NodeSplit(bucket_index);
#ifdef TIOCS_DEBUG
        PrintHashTable();
#endif
    }


    start = std::chrono::high_resolution_clock::now();
    for(int i=curLevel; i>=0; --i)
    {
        while( memcmp(temp->next[i]->key,hashkey,KEY_SIZE)<0)
        {
            temp = temp->next[i];
        }
        update[i] = temp;
    }
    temp = temp->next[0];
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> search_time = end - start;

    start = std::chrono::high_resolution_clock::now();
    if(memcmp(temp->key, hashkey, KEY_SIZE)==0)
    {
        if(temp->flag == 1)
        {
            return 0;
        }
        else
        {
            temp->flag =1;
            // write into disk
#ifdef NOT_SEPARATE_KV
            temp->offset = async_kv_write(hashkey,hashvalue); //printf("%u\n",temp->offset);
#elif defined(NOT_SEPARATE_KV_variable)
            temp->offset = async_kv_write(hashkey,hashvalue); //printf("%u\n",temp->offset);
#elif defined(SEPARATE_KV_FIXED_LOG)
            uint64_t block_id = 0;
            temp->offset = async_kv_separate_update(hashvalue,block_id);
            temp->block = block_id;
#elif defined(SEPARATE_KV_VARIABLE_LOG)
            uint64_t block_id = 0;
            temp->offset =0; // async_kv_separate_variable_update(hashvalue,block_id);
            temp->block = block_id;
#endif
            ++head->Nodenumber;
            return 0;
        }
    }
    else
    {
        int v=RandomLevel();
        if(v > curLevel )
        {
            for(int i=curLevel; i<v-1; ++i )
            {
                update[i] = head->HashNode;
            }
            head->CurrentLevel = v;
        }
#ifdef NOT_SEPARATE_KV
        uint64_t offset1 = async_write(hashkey,hashvalue); //printf("%u\n",temp->offset);
#elif defined(NOT_SEPARATE_KV_variable)
        uint64_t offset1 = async_kv_write(hashkey,hashvalue); //printf("%u\n",temp->offset);
#elif defined(SEPARATE_KV_FIXED_LOG)
        uint64_t block_id = 0;
        uint64_t offset1 = async_kv_separate_write(hashkey,hashvalue,block_id);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
        uint64_t block_id = 0;
        uint64_t offset1 = 0; //async_kv_separate_variable_write(hashkey,hashvalue,block_id); 
#endif 
        head->Nodenumber++;

#ifdef NOT_SEPARATE_KV
        temp = Initialization(hashkey,offset1,v); 
#elif defined(NOT_SEPARATE_KV_variable)
        temp = Initialization(hashkey,offset1,v);
#elif defined(SEPARATE_KV_FIXED_LOG)
        temp = Initialization(hashkey,offset1,block_id,v);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
        temp = Initialization(hashkey,offset1,block_id,v);
#endif
        
        head->Nodenumber++;
        for(int i=0;i<v;++i)
        {
            temp->next[i] = update[i]->next[i];
            update[i]->next[i] = temp;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    insert_and_write_time += end - start;

#endif
    return 0;
}


/**  
 * =================Node Searching module====================
 * There are 3 steps for searching whether a kv pair in the TNC-tree: 
 * 1. check whether the page in memory or not
 * 2. return value if data is not flushed into physical page, or read from physical page
 * 3. return value if data is in the physical block   
 **/
#ifndef FastSkiplist
TSkiplistNode * SearchNode(TNCSkiplist * Head,const char* hashkey)
{
  
    TSkiplistNode*node = Head->head;
    for(int i=Head->level; i>=0; i--)
    { 
        while(node->forward[i] && memcmp(node->forward[i]->key,hashkey,KEY_SIZE)<0)
        {
            node = node->forward[i];
        }
    }

    node = node->forward[0];
    return node;

    // printf("Bucket (depth: %d):\n", Head->depth);
    // TSkiplistNode *node2 = Head->head->forward[0];
    // int i=910;
    // while(i--)
    // {
    //     node2 = node2->forward[0];
    // }
    // i=10;
    // while (node2 && i--) 
    // {
    //     printf(" -Node with hash_key: %lu, offset: %u, flag: %u, level: %d\n", big_endian2little_endian(node2->key, KEY_SIZE), node2->offset, node2->flag, node2->maxLevel);
    //     node2 = node2->forward[0];
    // }
    // printf("hashkey: %lu\n",big_endian2little_endian(hashkey,KEY_SIZE));
  
}
#endif
#ifdef FastSkiplist
LocalHashNode * SearchNode(LocalHeadNode * Head,const char* hashkey)
{
    LocalHashNode*node = Head->head;
    for(int i=Head->level; i>=0; i--)
    { 
        while(node->next[i] && memcmp(node->next[i]->key,hashkey,KEY_SIZE)<0)
        {
            node = node->next[i];
        }
    }

    node = node->next[0];
    return node;
}
#endif


value_type Search(const char* key1)
{
#ifndef FastSkiplist
    char *entry1;
    // entry_t entry(KEY_SIZE,FLAGS_value_size);
    uint64_t key2 = big_endian2little_endian(key1,KEY_SIZE);
    TNCSkiplist * head = global[key2 & (1<<Globaldepth)-1]->local;
    TSkiplistNode * node =  SearchNode(head, key1);


    // printf("key:%lu in search\n",key2);

    if(node == nullptr || node->flag == 0)
    {
        return UINT64_MAX;
    }
    else
    {
        // entry1 = SyncRead(node->offset);

#ifdef TIOCS_READ_DEBUG
        printf("key from data: %lu\n",big_endian2little_endian(entry.val,KEY_SIZE));
        printf("Search - key1: %lu\n", big_endian2little_endian(key1, KEY_SIZE));
        printf("Search - node->key: %lu\n", big_endian2little_endian(node->key, KEY_SIZE));
        printf("Search - node->offset: %u (Block: %u, Page: %u, Position: %u)\n",
               node->offset,
               (node->offset >> 24),
               ((node->offset >> 12) & 0xFFF),
               (node->offset & 0x00000FFF));
#endif
        
        return  big_endian2little_endian(node->key,KEY_SIZE);
    }

#endif



#ifdef FastSkiplist
    
    uint64_t key2 = big_endian2little_endian(key1,KEY_SIZE);
    LocalHeadNode * head = global[key2 & (1<<Globaldepth)-1]->local;
    LocalHashNode * node =  SearchNode(head, key1);

    if(node == nullptr || node->flag == 0)
    {
        return UINT64_MAX;
    }
    else
    {
        // entry = SyncRead(node->offset);

#ifdef TIOCS_READ_DEBUG
        printf("key from data: %lu\n",big_endian2little_endian(entry.val,KEY_SIZE));
        printf("Search - key1: %lu\n", big_endian2little_endian(key1, KEY_SIZE));
        printf("Search - node->key: %lu\n", big_endian2little_endian(node->key, KEY_SIZE));
        printf("Search - node->offset: %u (Block: %u, Page: %u, Position: %u)\n",
               node->offset,
               (node->offset >> 24),
               ((node->offset >> 12) & 0xFFF),
               (node->offset & 0x00000FFF));
#endif
        
        // return big_endian2little_endian(entry.val,KEY_SIZE);
    }

#endif

}




/**  
 * ================= update module ====================  
 **/
int Update(const char* key1, const char* val)
{
    int err = 0;
    uint64_t key2 = big_endian2little_endian(key1,KEY_SIZE);

#ifndef FastSkiplist
    TNCSkiplist * head = global[key2 & (1<<Globaldepth)-1]->local;
    TSkiplistNode * node = SearchNode(head, key1);
#endif

#ifdef FastSkiplist
    LocalHeadNode * head = global[key2 & (1<<Globaldepth)-1]->local;
    LocalHashNode * node = SearchNode(head, key1);
#endif

    if(node == nullptr){
        return -1;
    }

    if(node->flag == 1)
    {
        //printf("key: %lu ",key1);
        // printf("offset: %u\n",node->offset);
#ifdef NOT_SEPARATE_KV
        err = SyncDelete(node->offset);
#elif defined(NOT_SEPARATE_KV_variable)
        err = SyncvariableDelete(node->offset);
#elif defined(SEPARATE_KV_FIXED_LOG)
        // int a = node->offset>>24;
        // if(big_endian2little_endian(key1,KEY_SIZE) == 8456562)
        // {
        //     printf("key1 :%lu offset:%u\n block:%u \n",big_endian2little_endian(node->key,KEY_SIZE),node->offset,node->block);
        // }
        err = SynckvDelete(node->offset);
        // if(big_endian2little_endian(key1,KEY_SIZE) == 8456562)
        // {
        //     printf("over=====\n");
        // }
#elif defined(SEPARATE_KV_VARIABLE_LOG)
        err = SynckvvariableDelete(node->offset);
#endif
    }
    
    if(err != 0 )
    {
        EMessageOutput("Update failure!",5899);
    }
    // printf("node->block:%d\n",node->block);
#ifdef NOT_SEPARATE_KV
    node->offset = SyncWrite(key1,val);
#elif defined(NOT_SEPARATE_KV_variable)
    node->offset = SyncWrite(key1,val);
#elif defined(SEPARATE_KV_FIXED_LOG)
    node->offset = SyncvseparateWrite(val,node->block);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
    node->offset = SyncvvariableseparateWrite(val,node->block);
#endif

    return 0;
}



/**
 *   =================Node deletion module====================  
 **/

#ifndef FastSkiplist
    bool DeleteValue(TNCSkiplist * Head, const char* hashkey)
#endif
#ifdef FastSkiplist
    bool DeleteValue(LocalHeadNode * Head, const char* hashkey)
#endif
{
#ifndef FastSkiplist
    TSkiplistNode * node = SearchNode(Head,hashkey);
#endif

#ifdef FastSkiplist
    LocalHashNode * node = SearchNode(Head,hashkey);
#endif
    
    if(node==nullptr)
    {
        return false;
    }
    if (node->flag == 0)
    {
        printf("The key is 0 in the TNC-tree!\n");
        return false;
    }
    

    if(node->flag==1)
    {
#ifdef NOT_SEPARATE_KV
        SyncDelete(node->offset);
#elif defined(NOT_SEPARATE_KV_variable)
        SyncvariableDelete(node->offset);
#elif defined(SEPARATE_KV_FIXED_LOG)
        SynckvDelete(node->offset);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
        SynckvvariableDelete(node->offset);
#endif
        //write into disk(meta data).
        node->flag=0;
    }

    return true;
}

int Delete(const char* key1)
{
    uint64_t key2 = big_endian2little_endian(key1,KEY_SIZE);
#ifndef FastSkiplist
    TNCSkiplist * head = global[key2 & (1<<Globaldepth)-1]->local;
#endif

#ifdef FastSkiplist
    LocalHeadNode * head = global[key2 & (1<<Globaldepth)-1]->local;
#endif
    
    bool flag = DeleteValue(head, key1);

    if(!flag)
    {
        EMessageOutput("Delete value failure in TNC-tree!",1578);
        exit(1578);
    }
    return 0;
}

/**
 *  =================Hash table initialization module====================  
 **/

int ExtendHashTableInitialize()
{

    /* Initialize a special hash table and SSD backend processor! */
   
    for(int i=0; i<20;i++)
    {
        if(!DoubleHashtable()){
            printf("Memory tier of TNC-tree initialized failure!");
            exit(5735);
        }
    }
    
    

    /**
     * printf("============== TEST & DEBUG: ==============\n");
     * for(int i=0;i<global.size();++i){
     * printf("Number of Global Node: %d current level: %d current number of nodes: %d  ",i+1,global[0]->local->CurrentLevel,global[0]->local->Nodenumber);
     * printf("NIL verification %u\n",global[i]->local->HashNode->next[9]->Hashvalue);
     * }
     **/
    return 0;

}
