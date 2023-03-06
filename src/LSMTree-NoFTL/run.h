/**
 * @date   2/3/2023
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
 *          TiOCS_experiment/TiOCS/src/LSMTree-NoFTL/run.h
 **/


#ifndef TiOCS_LSMTREENOFTL_RUN_H
#define TiOCS_LSMTREENOFTL_RUN_H

#include <vector>
#include <queue>
#include "../Auxizilary/VariablesTypes.h"
#include "../Backend/IODisk/WriteDisk.h"
#include "../Auxizilary/SysOutput.h"

class NoFTLRun 
{

private:
    uint64_t Size, MaxSize;
    uint64_t MaxKey, MinKey;
    std::vector<PageType> PagePointers;
    std::vector<entry_t> Rundata;
    std::vector<KEY_t> FencePointers;
    
public:
    NoFTLRun(uint64_t);
    int  RunDataWrite();
    void PointersDisplay();
    void PutValue(entry_t entry);
    std::vector<entry_t> RunValuesRead(uint64_t PageNum);
    std::vector<entry_t> SingleRunRead();
    VAL_t * GetValue(KEY_t key);
    std::vector<entry_t> * GetRange(KEY_t, KEY_t);
    std::vector<uint64_t> GetPagePointers(void);
    std::vector<KEY_t> GetFencePointers(void);
    uint64_t GetMaxKey(void);
    uint64_t GetMinKey(void);
    int SetPagePointers(std::vector<uint64_t>);
    int SetFencePointers(std::vector<KEY_t>);
    void Reset();
    void Reset(bool flag);
    int SetMaxkey(KEY_t);
    void Unbind();
    int DataClear(std::vector<entry_t> );
    unsigned long GetNowSize();
    bool Isfull(void);
    bool IsEmpty(void);

};

#endif