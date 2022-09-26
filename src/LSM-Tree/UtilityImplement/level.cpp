#include "../UtilityDefine/level.h"
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


Level::Level(long buffersize)
{

    this->LevelNumber = LevelAlloctor();
	this->MaxRuns = 2;
	this->MaxRunSize = buffersize * pow(2,LevelNumber-1);
    for(int i=0;i<MaxRuns;i++)
    {
        Run run(MaxRunSize);
        Runs.emplace_back(run);
    }

}

void Level::PutValue(entry_t entry1)
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

void Level::PutEntries(std::vector<entry_t> entry1)
{
    for(auto& run: Runs)
    {
        if(run.Isfull())
        {
            run.DataClear(entry1);
            return ;
        }
    }
}

bool Level::IsEmpty(void) 
{
    for(auto& run:Runs)
    {
        if(run.GetNowSize()!= 0 && run.GetNowSize() == MaxRunSize)
        {
            return false;
        }
    }

    return true;
}

bool Level::IsFull(void) 
{
    for(auto& run:Runs)
    {
        if(run.GetNowSize()!= MaxRunSize && run.GetNowSize() == 0)
        {
            return false;
        }
    }
    return true;
}

long Level::GetMRunSize(void) const
{
    return MaxRunSize;
}

long Level::GetLevelNumber(void) const
{
    return LevelNumber;
}