/**
 * @date   0:32 7/9/2022
 * @author zz.wang
 * @details
 *          The following code define "Level" class and "Run" class. 
 *          The first class storing datum from "buffer" or last "Level", every "level" has 2 or 2^i(i=2,3,4...) 
 *          "Runs" where the size equals size of buffer.
 *          Every Level has three parameters:
 *          LevelNumber: Number of a se4pecific Level.
 *          MaxRus: Maximum of Runs in a specific Level, it equals "2^LevelNumber(LevelNumber= 1,2,3,4...)" 
 *          MaxRunSize: Size of Run, it equals buffer size(In this OC-SSD mode(FEMU), the default value is 64(that means every Run can store 1MB datum)). 
 *          
 *
 * IDENTIFICATION:
 *          code2/code/LSM-Tree/lsm_tree.h
 **/

#ifndef EXPERIMENT1_LEVEL_H
#define EXPERIMENT1_LEVEL_H

#include <vector>
#include <queue>
#include "../../Auxizilary/VariablesTypes.h"

class Run 
{

private:
    long Size, MaxSize;
    uint64_t PagePointers[64];
    KEY_t MaxKey;
    std::vector<KEY_t> fence_pointers;
    entry_t *mapping;
    size_t mapping_length;
    int mapping_fd;
    
public:
    Run(long);
    ~Run(void);
    uint64_t  RunWrite();
    entry_t * RunRead();
    entry_t * map_read(size_t, off_t);
    entry_t * map_read(void);
    entry_t * map_write(void);
    void unmap(void);
    VAL_t * get(KEY_t);
    std::vector<entry_t> * range(KEY_t, KEY_t);
    void put(entry_t);
    long file_size() {return MaxSize * sizeof(entry_t);}
};


class Level 
{
private:
    int LevelNumber;
    int MaxRuns;
    long MaxRunSize;

public:
    std::deque<Run> Runs;
    Level(long s);
    bool remaining(void) const 
    { 
        return MaxRuns - Runs.size();
    }

};


#endif   // EXPERIMENT1_LEVEL_H