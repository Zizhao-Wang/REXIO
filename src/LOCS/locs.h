/**
 * @date    3/4/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/LOCS/locs.h
 */

#ifndef TiOCS_LOCS_LOCS_H
#define TiOCS_LOCS_LOCS_H

#include <vector>
#include "buffer.h"
#include "level.h"
#include "spin_lock.h"
#include "run.h"
#include "merge.h"
#include "../Auxizilary/pre_definition.h"
#include "../Auxizilary/SysOutput.h"


#define DEFAULT_THREAD_COUNT 1          // Single thread query
#define DEFAULT_BF_BITS_PER_ENTRY 0.5   // bloom filter bits


extern uint32_t LSMTreeReadPhysicalPage;
extern uint32_t LSMTreeWritePhysicalPage;
extern uint32_t LSMTreeErasehysicalPage;


class LOCS 
{

private:
    //float bf_bits_per_entry;
    // 7-levels 
    vector<locs_level> Levels;
    locs_buffer buffer;

public:
    LOCS(size_t ,int );
    int PutValue(const char* key, const char* value);
    const char* GetValue(const char* key);
    void GetRange(KEY_t, KEY_t);
    void UpdateValue(const char* key, const char* value);
    void DeleteValue(const char* key);
    void load(std::string);
    void display();
    int FlushInto(vector<locs_level>::iterator);
    locs_run * get_run(int);
};

void print_ocssd_geometry(struct spdk_ocssd_geometry_data *geometry_data);

void get_ocssd_geometry_completion(void *cb_arg, const struct spdk_nvme_cpl *cpl);


/* Portable function declaration */
void locs_init(void);

void locs_close(void);

#endif //TiOCS_LOCS_LOCS_H