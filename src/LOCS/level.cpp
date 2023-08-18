#include <cassert>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/types.h>
#include "level.h"
#include "allocation.h"

locs_level ::locs_level (uint64_t buffersize)
{
    this->LevelNumber = LevelRising();
	this->MaxRuns = 2;
	this->MaxRunSize = buffersize * pow(2,LevelNumber-1); //256*1024*2^(levelnumber-1)
    //printf("Run Maxsize:%lu in level %ld!\n",MaxRunSize,LevelNumber);
    for(int i=0;i<this->MaxRuns;i++)
    {
        locs_run run(MaxRunSize);
        Runs.emplace_back(run);
    }
}

void locs_level ::PutValue(entry_t entry1)
{
    for(auto& run: Runs)
    {
        if(!run.Isfull())
        {
            run.PutValue(entry1);
            return ;
        }
    }
}

const char* locs_level ::GetValue(const char* key)
{
    const char *val = nullptr;
    for(int i=0;i<Runs.size();i++)
    {  
       // printf("Search in Run %d, Run has %lu\n",i,Runs[i].GetNowSize());
        if(!Runs[i].IsEmpty())
        {
            val = Runs[i].GetValue(key);
            if(val != nullptr)
                return val;
        }
    }

    delete(val);
    return nullptr;
}

void locs_level ::PutEntries(std::vector<entry_t> entry1)
{
    for(auto ent: entry1)
    {
        PutValue(ent);
    }
}

bool locs_level ::IsEmpty(void)  
{
    for(auto& run:Runs)
    {
        if(run.GetNowSize()!= 0 )
        {
            return false;
        }
    }
    return true;
}

bool locs_level ::IsFull(void)  
{
    for(auto& run:Runs)
    {
        if(run.GetNowSize()!= MaxRunSize)
        {
            return false;
        }
    }
    return true;
}

long locs_level ::GetMRunSize(void) const
{
    return MaxRunSize;
}

long locs_level ::GetLevelNumber(void) const
{
    return LevelNumber;
}

uint64_t locs_level ::GetNowSize(void)
{
    uint64_t size = 0;
    for (int i=0;i<Runs.size();i++)
    {
        size += Runs[i].GetNowSize();
    }
    
    return size;
}

uint64_t locs_level ::GetMaxSize(void)
{
    
    return MaxRuns*MaxRunSize;
}

void locs_level ::PrintLevel(void)
{
    printf("Level %ld:\n",LevelNumber);
    for(int i=0;i<Runs.size();i++)
    {
        printf("Run %d: size: %ld\n ",i,Runs[i].GetNowSize());
    }
}