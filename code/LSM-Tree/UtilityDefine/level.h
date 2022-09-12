/**
 * @date   0:32 7/9/2022
 * @author zz.wang
 * @details
 *          This file shows how to construct a LSM-Tree.
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
    std::vector<KEY_t> fence_pointers;
    KEY_t max_key;
    entry_t *mapping;
    size_t mapping_length;
    int mapping_fd;
    long file_size() {return max_size * sizeof(entry_t);}
    
public:
    long size, max_size;
    std::string tmp_file;
    Run(long, float);
    ~Run(void);
    entry_t * map_read(size_t, off_t);
    entry_t * map_read(void);
    entry_t * map_write(void);
    void unmap(void);
    VAL_t * get(KEY_t);
    std::vector<entry_t> * range(KEY_t, KEY_t);
    void put(entry_t);
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