/**
 * @date:   1/3/2022
 * @author: zz.wang
 * @email:  zz.wang@siat.ac.cn
 * @details:
 *          This file shows how to construct NoFTL-KV (LSM-tree for native storage).
 *
 * IDENTIFICATION:
 *          src/LSM-Tree/LSMTree.h
 **/

#ifndef TiOCS_LSMTREENOFTL_STORAGEMANAGER_H
#define TiOCS_LSMTREENOFTL_STORAGEMANAGER_H

#include <vector>
#include "buffer.h"
#include "level.h"
// #include "UtilityDefine/spin_lock.h"
// #include "UtilityDefine/worker_pool.h"
// #include "../Auxizilary/VariablesTypes.h"
#include "level.h"
#include "merge.h"
#include "run.h"
// #include "../Auxizilary/SysOutput.h"



#define DEFAULT_TREE_DEPTH 5
#define DEFAULT_TREE_FANOUT 10
#define DEFAULT_BUFFER_NUM_PAGES 64     // 64 pages = 1MBytes
#define DEFAULT_THREAD_COUNT 1          // Single thread query
#define DEFAULT_BF_BITS_PER_ENTRY 0.5   // bloom filter bits


extern uint32_t rNoFTLKVPages;
extern uint32_t wNoFTLKVPages;
extern uint32_t eNoFTLKVPages;

class RegionParams
{

private:
    size_t channels_per_ssd;
    size_t blocks_per_channel;
    size_t io_requests;

public:
    RegionParams(size_t channels_per_ssd=32, size_t blocks_per_channel=2)
    {
        this->channels_per_ssd = channels_per_ssd;
        this->blocks_per_channel = blocks_per_channel;
        this->io_requests = channels_per_ssd * blocks_per_channel;
    }

    size_t get_io_requests()
    {
        return this->io_requests;
    }

};

class LSMTreeNoFTL 
{

private:
    memory_buffer buffer;
    RegionParams region_params;
    vector<NoFTLKVLevel> Levels;
    //float bf_bits_per_entry;
    // 7-levels 

public:
    LSMTreeNoFTL(size_t ,int );
    int PutValue(KEY_t, VAL_t);
    VAL_t* GetValue(KEY_t);
    void GetRange(KEY_t, KEY_t);
    void UpdateValue(KEY_t, VAL_t);
    void DeleteValue(KEY_t);
    void load(std::string);
    void display();
    int FlushInto(vector<NoFTLKVLevel>::iterator);
    NoFTLRun * get_run(int);
};


void NoFTLKVInit(void);



#endif //EXPERIMENT1_LSMTREE_H