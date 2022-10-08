#include "node2.h"
#include <iostream>
#include <stdlib.h>

SkiplistNode *skiplistNodeCreat(uint64_t key,uint64_t val, int maxLevel) 
{
    SkiplistNode *obj = (SkiplistNode *)malloc(sizeof(SkiplistNode));
    obj->value = val;
	obj->key = key;
    obj->maxLevel = maxLevel;
    obj->forward = (SkiplistNode **)malloc(sizeof(SkiplistNode *) * maxLevel);
    for (int i = 0; i < maxLevel; i++) {
        obj->forward[i] = NULL;
    }
    return obj;
}


Skiplist* skiplistCreate() 
{
    Skiplist *obj = (Skiplist *)malloc(sizeof(Skiplist));
    obj->head = skiplistNodeCreat(0,0, MAX_LEVEL);
    obj->level = 0;
    srand(time(NULL));
    return obj;
}


LSMEntry * NILInit()
{
    auto * nil =(LSMEntry *) malloc(sizeof(LSMEntry));
    if(nil!= nullptr)
        nil->key=UINT64_MAX;
    return nil;
}


LSMEntry * Init() 
{

    LSMEntry *local = (LSMEntry *)malloc(sizeof(LSMEntry));
    if (local == NULL)
        exit(0);
	local ->key = 0;
	local ->val = 0;
	local ->val = 32;
    for(int i=0;i<1;i++)
    {
        local->next[i]= NULL;
    }
    return local;
}



LSMEntry * Initialize(uint64_t hashkey, uint64_t val1,int maxlevel)
{
	auto * temp =(LSMEntry*) malloc(sizeof(LSMEntry));
    if(temp == nullptr)
        exit(0);

    temp->key = hashkey;
	temp->val = val1;
	temp->maxlevel = maxlevel;

	for(int i=0;i<maxlevel;i++)
	{
		temp->next[i]=nullptr;
	}
   
    return temp;
}