/**
 * @date   0:32 7/9/2022
 * @author zz.wang
 * @details
 *          This file shows how to construct a LSM-Tree.
 *
 * IDENTIFICATION:
 *          code2/code/LSM-Tree/lsm_tree.h
 **/

#ifndef EXPERIMENT1_LSMTREE_H
#define EXPERIMENT1_LSMTREE_H


#include "UtilityDefine/buffer.h"
#include "UtilityDefine/level.h"
#include "UtilityDefine/spin_lock.h"
#include "UtilityDefine/worker_pool.h"
#include "../Auxizilary/VariablesTypes.h"
#include <vector>


#define DEFAULT_TREE_DEPTH 5
#define DEFAULT_TREE_FANOUT 10
#define DEFAULT_BUFFER_NUM_PAGES 64     // 64 pages = 1MBytes
#define DEFAULT_THREAD_COUNT 1          // Single thread query
#define DEFAULT_BF_BITS_PER_ENTRY 0.5   // bloom filter bits


class LSMTree 
{
private:
    Buffer buffer;
    WorkerPool worker_pool;
    float bf_bits_per_entry;
    vector<Level> levels;
    Run * get_run(int);
    void FlushInto(vector<Level>::iterator);

public:
    LSMTree(int BufferSize, int fanout, int NumThreads, float BitsPerEntry):
        bf_bits_per_entry(BitsPerEntry),buffer(BufferSize),worker_pool(NumThreads);
    int PutValue(KEY_t, VAL_t);
    void GetValue(KEY_t);
    void GetRange(KEY_t, KEY_t);
    void DeleteValue(KEY_t);
    void load(std::string);
};


/* Portable function declaration */
void LSMTreeInit();

#endif //EXPERIMENT1_LSMTREE_H