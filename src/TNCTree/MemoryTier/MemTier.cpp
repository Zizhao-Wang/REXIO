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
#include "../../Backend/IODisk/WriteDisk.h"
#include "../../Backend/BlockManage/BlockInfo.h"

#define N 9999

using namespace std;


/**
 *  =================Node initialization module==================== 
 **/

bool LocalGeneration(GlobalHashNode * globalNode)
{

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
        fork();
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
 * =================Node Searching module====================
 * There are 3 steps for searching whether a kv pair in the TNC-tree: 
 * 1. check whether the page in memory or not
 * 2. return value if data is not flushed into physical page, or read from physical page
 * 3. return value if data is in the physical block   
 **/

int Search()
{
    return 0;
}

LocalHashNode* SearchNode(LocalHeadNode* Head,unsigned int hashvalue)
{
    if(Head->CurrentLevel == 1)
        return nullptr;
    int curLevel = Head->CurrentLevel-1;
    LocalHashNode *node = Head->HashNode ;
    for(int i=curLevel; i>=0; --i)
    {
        while(node->next[i]->Hashvalue < hashvalue)
        {
            node = node->next[i];
        }
    }

    node = node->next[0];
    return node;
}


/**
 *  =================Node Insertion module====================  
 **/

int RandomLevel()
{

    int v = 1;
    srand ((unsigned int)(time (NULL)));
    while(rand() % (N + 1) / (float)(N + 1) < pro && v< MaxLevel )
    {
        v++;
    }
#ifdef DEBUG

#endif
    return v;

}

/*LocalHeadNode * NodeSplit(LocalHeadNode * head)
{


}*/

bool InsertNode(uint64_t hashvalue)
{
    /*
     *  Local variables.
     */
    LocalHeadNode * head = global[hashvalue & (1<<Globaldepth)-1]->local;
    //printf("%d\n",hashvalue&(1<<Globaldepth)-1);
    LocalHashNode * temp = head->HashNode;
    LocalHashNode * update[MaxLevel];

    // level
    int curLevel = head->CurrentLevel-1;


    for(int i=curLevel; i>=0; --i)
    {
        while(temp->next[i]->Hashvalue < hashvalue)
        {
            temp = temp->next[i];
        }
        update[i] = temp;
    }
    temp = temp->next[0];

    if(temp->Hashvalue == hashvalue)
    {
        if(temp->flag == 1)
        {
            return true;
        }
        else
        {
            temp->flag =1;
            // write into disk
            temp->offset = SSD_write(hashvalue);
            //printf("%u\n",temp->offset);
            ++head->Nodenumber;
            return true;
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
        uint_32 offset1 = SSD_write(hashvalue);
        //printf("%u\n",temp->offset);
        ++head->Nodenumber;
        temp = Initialization(hashvalue,offset1);
        if(temp == nullptr)
            return false;
        for(int i=0;i<v;++i)
        {
            temp->next[i] = update[i]->next[i];
            update[i]->next[i] = temp;
        }
    }
    return true;
}

int Insert(SKey hashvalue, SValue val)
{

    /*  Insert the hash value into special skip-list. */
    if(InsertNode(hashvalue))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**
 *   =================Node deletion module====================  
 **/

bool DeleteNode(LocalHeadNode * Head, unsigned int hashvalue)
{
    LocalHashNode * node = SearchNode(Head,hashvalue);
    if(node->flag!=0)
    {
        //write into disk(meta data).
        node->flag=0;
    }
    return true;
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
