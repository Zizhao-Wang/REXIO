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
#include <iostream>
#include "../utility/types.h"
#include "../StoreTier/syncstore.h"
#include "../../Auxizilary/SysOutput.h"
#include "../../Debug/debug_micros.h"

#define N 9999

using namespace std;


/**
 *  =================Node initialization module==================== 
 **/

bool LocalGeneration(GlobalHashNode *globalNode, int initialLocalDepth=0) 
{
    TNCSkiplist *localHead = TskiplistCreate();
    if( initialLocalDepth != 0)
    {
        localHead->depth = initialLocalDepth;
    }
    else
    {
        localHead->depth = Globaldepth+1;
    }

    globalNode->local = localHead;

#ifdef FastSkiplist
    auto * localHead =(LocalHeadNode*) malloc(LOCAL_HEAD_SIZE);
    if(localHead!=nullptr || globalNode->local== nullptr)
    {
        localHead->depth  = Globaldepth;
        localHead->Nodenumber = 0;
        localHead->CurrentLevel = 1;

        auto * localnodehead = Initialization();
        localHead->HashNode = localnodehead;

        globalNode->local = localHead;
        return true;
    }
    else
        return false;
#endif

    return true;
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

uint64_t big_endian2little_endian(const char *big_endian, size_t len)
{
    uint64_t result = 0;
    for (size_t i = 0; i < len; ++i) 
	{
        result |= (static_cast<uint64_t>(static_cast<unsigned char>(big_endian[i])) << (8 * (len - 1 - i)));
    }
    return result;

}



/**
 *  =================Node Insertion module====================  
 **/

int RandomLevel()
{
    int v = 1;
    while (rand() < P_FACTOR1 && v < MAX_LEVEL1) {
        v++;
    }
#ifdef FastSkiplist
    int v = 1;
    srand ((unsigned int)(time (NULL)));
    while(rand() % (N + 1) / (float)(N + 1) < pro && v< MaxLevel )
    {
        v++;
    }
#endif
    return v;

}

void  NodeSplit(size_t bucket_index)
{
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

#ifdef TIOCS_DEBUG
        printf("Processing hash_key: %lu\n", hash_key);
#endif
        if (hash_key & split_boundary) 
        {
            int new_index = bucket_index | split_boundary;
            TNCSkiplist *new_bucket = global[new_index]->local;
#ifdef TIOCS_DEBUG
            printf("Inserting hash_key %lu into new bucket %d\n", hash_key, new_index);
#endif
            InsertNode(node->key, node->offset, node->flag, new_index);
        } 
        else 
        {
#ifdef TIOCS_DEBUG
            printf("Inserting hash_key %lu back into old bucket %zu\n", hash_key, bucket_index);
#endif
            InsertNode(node->key, node->offset, node->flag, bucket_index);
        }
        node = next;
    }
}

int InsertNode(const char* hashkey, uint32_t offset, uint8_t flag, int bucket_id)
{

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
    if(v > Head->level)
    {
        for(int i=Head->level; i<v; ++i )
        {
                update[i] = Head->head;
        }
        Head->level = v;
    }
    ++Head->number;

#ifdef TIOCS_DEBUG
    printf("Inserting new node with hash_key: %.*s, offset: %u, flag: %u, level: %d\n",KEY_SIZE, hashkey, offset, flag, v);
#endif
    
    TSkiplistNode* tsl = TskiplistNodeCreat(hashkey,offset,v);
    tsl->flag = flag;
    if( big_endian2little_endian(hashkey,KEY_SIZE) == 117334)
    {
        printf(" offset in insertnode: %d\n", tsl->offset);
        
    }
    for(int i=0;i<v;++i)
    {
        tsl->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = tsl;
    }
    


#ifdef FastSkiplist 
    LocalHeadNode * head = global[hashkey & (1<<Globaldepth)-1]->local;
    LocalHashNode * temp = head->HashNode;
    LocalHashNode * update[MaxLevel];

    int curLevel = head->CurrentLevel-1;

    for(int i=curLevel; i>=0; --i)
    {
        while(temp->next[i]->Hashkey < hashvalue)
        {
            temp = temp->next[i];
        }
        update[i] = temp;
    }
    temp = temp->next[0];

    if(temp->Hashkey == hashvalue)
    {
        if(temp->flag == 1)
        {
            return 0;
        }
        else
        {
            temp->flag =1;
            // write into disk
            temp->offset = SyncWrite(hashkey,hashvalue); //printf("%u\n",temp->offset);
            ++head->Nodenumber;
            return 0;
        }
    }
    else
    {
        int v=RandomLevel();
        if(v > curLevel )
        {
            for(int i=curLevel+1; i<v; ++i )
            {
                update[i] = head->HashNode;
            }
            head->CurrentLevel = v+1;
        }
        //write into disk
        uint_32 offset1 = SyncWrite(hashkey,hashvalue);  //printf("%u\n",temp->offset);
        ++head->Nodenumber;
        temp = Initialization(hashvalue,offset1);
        if(temp == nullptr)
            return -1;
        for(int i=0;i<v;++i)
        {
            temp->next[i] = update[i]->next[i];
            update[i]->next[i] = temp;
        }
    }
#endif
    return 0;
}

void PrintHashTable() 
{
    printf("==== Hash Table Content ====\n");
    for (int i = 0; i < (1 << Globaldepth); i++) 
    {
        TNCSkiplist *bucket = global[i]->local;
        printf("Bucket %d (depth: %d):\n", i, bucket->depth);
        TSkiplistNode *node = bucket->head->forward[0];

        while (node) 
        {
            printf(" -Node with hash_key: %lu, offset: %u, flag: %u, level: %d\n", big_endian2little_endian(node->key, KEY_SIZE), node->offset, node->flag, node->maxLevel);
            node = node->forward[0];
        }
    }
    printf("=============================\n");
}

int InsertNode(const char* hashkey, const char* hashvalue)
{

    /*  Insert the hash value into special skip-list. */
    uint64_t hashkey1 = big_endian2little_endian(hashkey,KEY_SIZE);
    int bucket_index = hashkey1 & (1<<Globaldepth)-1;
    TNCSkiplist * Head = global[hashkey1 & (1<<Globaldepth)-1]->local;
    TSkiplistNode * temp = Head->head;
    TSkiplistNode * update[MAX_LEVEL1];
    bool flag= false;
    if(hashkey1 == 117334)
    {
        flag = true;
    }

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


    for(int i=Head->level; i>=0; i--)
    {
        while(temp->forward[i] && memcmp(temp->forward[i]->key,hashkey,KEY_SIZE)<0)
        {
            temp = temp->forward[i];
        }
        update[i] = temp;
    }

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
            temp->offset = SyncWrite(hashkey,hashvalue); //printf("%u\n",temp->offset);
            ++Head->number;
            return 0;
        }
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
            Head->level = v;
        }
        //write into disk
        uint_32 offset1 = SyncWrite(hashkey,hashvalue);  //printf("%u\n",temp->offset);
        ++Head->number;
        TSkiplistNode* tsl = TskiplistNodeCreat(hashkey,offset1,v);
        if(flag)
        {
            printf("offset in insert: %d \n", tsl->offset);
        }
        for(int i=0;i<v;++i)
        {
            tsl->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = tsl;
        }
    }

#ifdef FastSkiplist 
    LocalHeadNode * head = global[hashkey & (1<<Globaldepth)-1]->local;
    LocalHashNode * temp = head->HashNode;
    LocalHashNode * update[MaxLevel];

    int curLevel = head->CurrentLevel-1;

    for(int i=curLevel; i>=0; --i)
    {
        while(temp->next[i]->Hashkey < hashvalue)
        {
            temp = temp->next[i];
        }
        update[i] = temp;
    }
    temp = temp->next[0];

    if(temp->Hashkey == hashvalue)
    {
        if(temp->flag == 1)
        {
            return 0;
        }
        else
        {
            temp->flag =1;
            // write into disk
            temp->offset = SyncWrite(hashkey,hashvalue); //printf("%u\n",temp->offset);
            ++head->Nodenumber;
            return 0;
        }
    }
    else
    {
        int v=RandomLevel();
        if(v > curLevel )
        {
            for(int i=curLevel+1; i<v; ++i )
            {
                update[i] = head->HashNode;
            }
            head->CurrentLevel = v+1;
        }
        //write into disk
        uint_32 offset1 = SyncWrite(hashkey,hashvalue);  //printf("%u\n",temp->offset);
        ++head->Nodenumber;
        temp = Initialization(hashvalue,offset1);
        if(temp == nullptr)
            return -1;
        for(int i=0;i<v;++i)
        {
            temp->next[i] = update[i]->next[i];
            update[i]->next[i] = temp;
        }
    }
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

TSkiplistNode * SearchNode(TNCSkiplist * Head,const char* hashkey)
{

    TSkiplistNode*node = Head->head;

    printf("Bucket (depth: %d):\n", Head->depth);
    TSkiplistNode *node2 = Head->head->forward[0];
    int i=910;
    while(i--)
    {
        node2 = node2->forward[0];
    }
    i=10;
    while (node2 && i--) 
    {
        printf(" -Node with hash_key: %lu, offset: %u, flag: %u, level: %d\n", big_endian2little_endian(node2->key, KEY_SIZE), node2->offset, node2->flag, node2->maxLevel);
        node2 = node2->forward[0];
    }
    printf("hashkey: %lu\n",big_endian2little_endian(hashkey,KEY_SIZE));

    for(int i=Head->level-1; i>=0; i--)
    {
        printf("for test ");
        while(node->forward[i] && memcmp(node->forward[i]->key,hashkey,KEY_SIZE)<0)
        {
            printf("dnjv");
            node = node->forward[i];
        }
    }

    node = node->forward[0];
    return node;
}

SValue Search(const char* key1)
{

    TNCEntry entry;
    uint64_t key2 = big_endian2little_endian(key1,KEY_SIZE);
    
    TNCSkiplist * head = global[key2 & (1<<Globaldepth)-1]->local;
    printf("index: %lu\n", key2 & (1<<Globaldepth)-1);
    TSkiplistNode * node =  SearchNode(head, key1);

    if(node == nullptr || node->flag == 0)
    {
        return UINT64_MAX;
    }
    else
    {
        printf("Search - key1: %lu\n", big_endian2little_endian(key1, KEY_SIZE));
        printf("Search - node->key: %lu\n", big_endian2little_endian(node->key, KEY_SIZE));
        printf("Search - node->offset: %u (Block: %u, Page: %u, Position: %u)\n",
               node->offset,
               (node->offset >> 24),
               ((node->offset >> 12) & 0xFFF),
               (node->offset & 0x00000FFF));
        entry = SyncRead(node->offset);
        return big_endian2little_endian(entry.val,KEY_SIZE);
    }

    // //printf("Key: %lu  ReadKey:%lu \n",key1,entry.key);
    // if(memcmp(entry.key,key1,KEY_SIZE) ==0)
    // {
    //     return big_endian2little_endian(entry.val,KEY_SIZE);
    // }
    // // else
    // // {
    // //     printf("key1: %lu offset: %u entry.key: %lu Value: %lu\n",key1,node->offset,entry.key,entry.val);
    // //     exit(0);
    // // }

    // return UINT64_MAX;
}


/**  
 * ================= update module ====================  
 **/
int Update(const char* key1, const char* val)
{
    int err = 0;
    uint64_t key2 = big_endian2little_endian(key1,KEY_SIZE);
    TNCSkiplist * head = global[key2 & (1<<Globaldepth)-1]->local;
    TSkiplistNode * node = SearchNode(head, key1);

    if(node == nullptr){
        return -1;
    }

    if(node->flag == 1){
        //printf("key: %lu ",key1);
        err = SyncDelete(node->offset);
    }
    
    if(err != 0 )
    {
        EMessageOutput("Update failure!",5899);
    }
    node->offset = SyncWrite(key1,val);

    return 0;

}



/**
 *   =================Node deletion module====================  
 **/

bool DeleteValue(TNCSkiplist * Head, const char* hashkey)
{

    TSkiplistNode * node = SearchNode(Head,hashkey);
    if(node->flag==1)
    {
        SyncDelete(node->offset);   //write into disk(meta data).
        node->flag=0;
    }

    return true;
}

int Delete(const char* key1)
{
    uint64_t key2 = big_endian2little_endian(key1,KEY_SIZE);
    TNCSkiplist * head = global[key2 & (1<<Globaldepth)-1]->local;
    bool flag = DeleteValue(head, key1);
    if(!flag)
    {
        EMessageOutput("Delete value failure in TNC-tree!",1578);
    }
    return 0;
}

/**
 *  =================Hash table initialization module====================  
 **/

int ExtendHashTableInitialize()
{

    /* Initialize a special hash table and SSD backend processor! */
   

    if(! DoubleHashtable() )
    {
        printf("Memory tier of TNC-tree initialized failure!");
        exit(5735);
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
