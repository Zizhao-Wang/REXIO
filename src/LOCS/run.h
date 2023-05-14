/**
 * @date   19/9/2022
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
 *          src/LSM-Tree/UtilityDefine/run.h
 **/
#ifndef EXPERIMENT1_RUN_H
#define EXPERIMENT1_RUN_H

#include <vector>
#include <queue>
#include "../Auxizilary/pre_definition.h"
#include "../Auxizilary/SysOutput.h"




class locs_run 
{
private:
    uint64_t Size, MaxSize, max_io_size, io_count, max_entries_per_vector,current_vector_index;
    uint64_t thread_num;
    char max_key[KEY_SIZE];
    char min_key[KEY_SIZE];
    std::vector<uint64_t> chunk_pointers;
    std::vector<std::vector<entry_t>> Rundata; // 改变了这里
    std::vector<char*> FencePointers;
   
public:
    locs_run(uint64_t);
    uint64_t  RunDataWrite(size_t index);
    void PointersDisplay();
    void PutValue(entry_t entry);
    std::vector<entry_t> RunValuesRead(uint64_t PageNum);
    std::vector<entry_t> SingleRunRead();
    const char * GetValue(const char* key);
    std::vector<entry_t> * GetRange(KEY_t, KEY_t);
    std::vector<uint64_t> GetPagePointers(void);
    std::vector<char*> GetFencePointers(void);
    const char * get_max_key(void);
    const char * get_min_key(void);
    int set_chunk_pointers(std::vector<uint64_t>);
    int SetFencePointers(std::vector<char*>);
    void Reset();
    void set_max_key(const char* key);
    void set_min_key(const char* key);
    void set_current_size(uint64_t);
    void Unbind();
    int DataClear(std::vector<entry_t> );
    unsigned long GetNowSize();
    void chunk_reset();
    int status(void);
    void status_display(void);
    bool Isfull(void);
    bool IsEmpty(void);
};

struct thread_params
{
    locs_run* object;
    size_t index;
    uint64_t write_count;
};


#endif