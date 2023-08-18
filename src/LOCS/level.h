/**
 * @date   9/4/2023
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
 *          src/LOCS/level.h
 **/

#ifndef EXPERIMENT1_LSMTREE_LEVEL_H
#define EXPERIMENT1_LSMTREE_LEVEL_H

#include <vector>
#include <queue>
#include "../Auxizilary/pre_definition.h"
#include "../Auxizilary/SysOutput.h"
#include "run.h"

class locs_level 
{
private:
    int  MaxRuns;
    long MaxRunSize;
    long LevelNumber;

public:
    std::vector<locs_run> Runs;
    locs_level(uint64_t);
    void PutValue(entry_t entry1);
    const char* GetValue(const char* key);
    void PutEntries(std::vector<entry_t>);
    bool IsEmpty(void);
    bool IsFull (void);
    void PrintLevel(void);
    long GetMRunSize(void) const;
    long GetLevelNumber(void) const;
    uint64_t GetNowSize(void);
    uint64_t GetMaxSize(void);

};


#endif   // EXPERIMENT1_LEVEL_H