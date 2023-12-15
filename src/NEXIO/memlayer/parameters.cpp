#include "parameters.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>
#include <pthread.h>
#include "include/sys_output.h"
#include "memlayer/MemTier.h"
#include "memlayer/syncstore.h"
#include "disklayer/io_manager.h"
#include "memlayer/parameters.h"
#include "include/buffer.h"
#include "disklayer/write_buffer.h"
#include "include/spdk_env_init.h"
#include "utility/types.h"
#include "tests/nexio_test.h"





int initialize_extend_hash_table() 
{
    int Createflag = ExtendHashTableInitialize();
    if(Createflag != 0) 
    {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m ExtendHashTableInitialize failed.\n");
        return -1;
    }
    fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m Extendible hash table initialized successfully!\n");
    return 0;
}


// Initialize geometry and buffer related parameters
int initialize_geometry_and_buffer_parameters() 
{
    // for (uint64_t i = 0; i < geometry.num_grp*geometry.num_pu*geometry.num_chk; ++i) 
    // {
    //     BlockWritePointers[i] = 0;
    //     block_information[i] = std::make_pair(0,0);
    // }

    num_data_page = 896;
    
    combined_kv_buffer_init();
    init_io_manager();
    kv_buffer_init();

    size_t kv_pairs_per_lba = device_info->ns_info_array[0].lba_size/(KEY_SIZE+value_size);
    size_t logs_per_lba = device_info->ns_info_array[0].lba_size/(sizeof(uint32_t));
    size_t pages_per_block = my_controller.nexio_lba_uint;
    // // num_data_page = (pages_per_block - (pages_per_block)/(logs_per_page/kv_pairs_per_page +1))-240;

    // size_t keys_per_page = sectors_per_page*geometry.clba/(KEY_SIZE);
    // key_num_data_page = num_data_page*kv_pairs_per_page;

    // size_t values_per_page = sectors_per_page*geometry.clba/(VAL_SIZE);
    // value_num_data_page = num_data_page*kv_pairs_per_page;
    fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m Geometry and buffer parameters initialized successfully!\n");
    return 0;
}

// For SPDK I/O队列
int initialize_nvme_controller_and_io_qpair() 
{
    if(nvme_ssd_environment_init() != 0)
    {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Variables initialization failure!\n");
        return -1;
    }
    if(ctrlr == NULL) {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m ctrlr is NULL!\n");
        return -1;
    }
    qpair = spdk_nvme_ctrlr_alloc_io_qpair(ctrlr, NULL, 0);
    if(qpair == NULL) {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m spdk_nvme_ctrlr_alloc_io_qpair() failed\n");
        return -1;
    }
    fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m NVMe controller and I/O qpair initialized successfully!\n");
    return 0;
}


int initialize_system() 
{
    if(initialize_extend_hash_table() != 0) {
        return -1;
    }

    if(initialize_nvme_controller_and_io_qpair() != 0) {
        return -1;
    }

    if(initialize_geometry_and_buffer_parameters() != 0) {
        return -1;
    }
    return 1;
}