#include "parameters.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>
#include <pthread.h>
#include "include/SysOutput.h"
// #include "../LOCS/global_variables.h"
#include "memlayer/MemTier.h"
#include "memlayer/syncstore.h"
#include "disklayer/io_manager.h"
#include "memlayer/parameters.h"




int initialize_extend_hash_table() {
    int Createflag = ExtendHashTableInitialize();
    if(Createflag != 0) {
        printf("\x1b[31mERROR: ExtendHashTableInitialize failed.\x1b[0m\n");
        return -1;
    }
    printf("\x1b[32m---- Extend hash table initialized successfully!\x1b[0m\n");
    return 0;
}

// 初始化几何和缓冲区相关参数
int initialize_geometry_and_buffer_parameters() 
{
    sectors_per_page = geometry.ws_min;
    buffer_capacity = sectors_per_page * geometry.clba / (KEY_SIZE+VAL_SIZE);
    for (uint64_t i = 0; i < geometry.num_grp*geometry.num_pu*geometry.num_chk; ++i) 
    {
        BlockWritePointers[i] = 0;
        block_information[i] = make_pair(0,0);
    }

    buffer_init();
    kv_buffer_init();


    size_t kv_pairs_per_page = sectors_per_page*geometry.clba/(KEY_SIZE+VAL_SIZE);
    size_t logs_per_page = sectors_per_page*geometry.clba/(sizeof(uint32_t));
    size_t pages_per_block = geometry.clba/sectors_per_page;
    // num_data_page = (pages_per_block - (pages_per_block)/(logs_per_page/kv_pairs_per_page +1))-240;
    num_data_page = 800;

    size_t keys_per_page = sectors_per_page*geometry.clba/(KEY_SIZE);
    key_num_data_page = num_data_page*kv_pairs_per_page;

    size_t values_per_page = sectors_per_page*geometry.clba/(VAL_SIZE);
    value_num_data_page = num_data_page*kv_pairs_per_page;
    printf("\x1b[32m---- Geometry and buffer parameters initialized successfully!\x1b[0m\n");
    return 0;
}

// 初始化NVMe控制器和I/O队列
int initialize_nvme_controller_and_io_qpair() 
{
    if(ctrlr == NULL) {
        printf("\x1b[31mERROR: ctrlr is NULL!\x1b[0m\n");
        return -1;
    }
    qpair = spdk_nvme_ctrlr_alloc_io_qpair(ctrlr, NULL, 0);
    if(qpair == NULL) {
        printf("\x1b[31mERROR: spdk_nvme_ctrlr_alloc_io_qpair() failed\x1b[0m\n");
        return -1;
    }
    printf("\x1b[32m---- NVMe controller and I/O qpair initialized successfully!\x1b[0m\n");
    return 0;
}

int print_init_info() 
{
    if(initialize_extend_hash_table() != 0) {
        return -1;
    }

    if(initialize_geometry_and_buffer_parameters() != 0) {
        return -1;
    }

    if(initialize_nvme_controller_and_io_qpair() != 0) {
        return -1;
    }

    //TODO: Compute the buffer size
    MAX_BUFFER_TASKS = 1;

    printf("\x1b[34m\n=============== Index initialization complete ===============\x1b[0m\n");
    return 0;
}





int print_init_info(void)
{

     //TODO: Compute the buffer size
     MAX_BUFFER_TASKS = 1;

	
     printf("\n ================ Index information ================ \
            \n ---- child process created successful! \
            \n ---- In-memory table in main process Initializated successful! \
            \n ---- The I/O queue is initialized successful! \
            \n ---- The buffer is initialized successful! \
            \n ---- Meta data space allocated successful! \
            \n");

     return 0;

}