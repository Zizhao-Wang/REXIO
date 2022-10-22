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

#define N 9999

using namespace std;


/**
 *  =================Node initialization module==================== 
 **/

bool LocalGeneration(GlobalHashNode * globalNode)
{

    TNCSkiplist * localHead = TskiplistCreate();
    
    localHead->depth  = Globaldepth;

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
    int nex = global.empty()?2:(int)global.size();

    for(int i=0; i<nex; ++i)
    {
        auto * globalNode = (GlobalHashNode*) malloc(GLOBAL_HASH_SIZE);
        if(!LocalGeneration(globalNode))
            return false;
        global.push_back(globalNode);
    }
    ++Globaldepth;

    return true;
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

/*LocalHeadNode * NodeSplit(LocalHeadNode * head)
{


}*/

int InsertNode(SKey hashkey, SValue hashvalue)
{

    /*  Insert the hash value into special skip-list. */
    TNCSkiplist * Head = global[hashkey & (1<<Globaldepth)-1]->local;
    TSkiplistNode * temp = Head->head;
    TSkiplistNode * update[MAX_LEVEL1];


    for(int i=Head->level; i>=0; i--)
    {
        while(temp->forward[i] && temp->forward[i]->key < hashkey)
        {
            temp = temp->forward[i];
        }
        update[i] = temp;
    }

    if(temp->key == hashkey)
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

TSkiplistNode * SearchNode(TNCSkiplist * Head,SKey hashkey)
{

    TSkiplistNode*node = Head->head ;
    for(int i=Head->level-1; i>=0; i--)
    {
        while(node->forward[i] && node->forward[i]->key < hashkey)
        {
            node = node->forward[i];
        }
    }

    node = node->forward[0];
    return node;
}

SValue Search(SKey key1)
{

    TNCEntry entry;
    TNCSkiplist * head = global[key1 & (1<<Globaldepth)-1]->local;
    TSkiplistNode * node =  SearchNode(head, key1);
    if(node == nullptr || node->flag == 0)
    {
        return UINT64_MAX;
    }

    entry = SyncRead(node->offset);
    //printf("Key: %lu  ReadKey:%lu \n",key1,entry.key);
    if(entry.key == key1)
    {
        return entry.val;
    }
    // else
    // {
    //     printf("key1: %lu offset: %u entry.key: %lu Value: %lu\n",key1,node->offset,entry.key,entry.val);
    //     exit(0);
    // }

    return UINT64_MAX;
}


/**  
 * ================= update module ====================  
 **/
int Update(SKey key1, SValue val)
{
    int err = 0;
    TNCSkiplist * head = global[key1 & (1<<Globaldepth)-1]->local;
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

bool DeleteValue(TNCSkiplist * Head, SKey hashkey)
{
    TSkiplistNode * node = SearchNode(Head,hashkey);
    if(node->flag==1)
    {
        SyncDelete(node->offset);   //write into disk(meta data).
        node->flag=0;
    }

    return true;
}

int Delete(SKey key1)
{
    TNCSkiplist * head = global[key1 & (1<<Globaldepth)-1]->local;
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

    for(int i=0;i<20;i++)
    {
        if(! DoubleHashtable() )
        {
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
