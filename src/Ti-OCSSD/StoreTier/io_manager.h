/**
 * @date    8/5/2023 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/Ti-OCSSD/StoreTier/io_manager.h
 */

#ifndef TIOCS_TIOCSSD_IO_MANAGER_H
#define TIOCS_TIOCSSD_IO_MANAGER_H

#include <iostream>
#include "../MemoryTier/Node.h"

/**
 *  ================= I/O records====================  
 **/
extern int reads;

extern int writes;

extern int resets;

extern int out_stand;

extern size_t num_data_page;

extern uint64_t page_pointer;

extern struct spdk_nvme_qpair *qpair;

extern std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkLog;

extern std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkData;

extern std::unordered_map<uint64_t, uint64_t> BlockWritePointers;


#define OCSSD_DATA_WRITE_TIOCS 1

#define OCSSD_LOG_WRITE_TIOCS  2



/**  
 * ================= DATA WRITE(I/O)  module ====================  
 **/
int write_queue();



/**  
 * ================= LOG WRITE(I/O) module ====================  
 **/
int write_queue(char *write_buffer, uint64_t block_id);



/**  
 * ================= READ(I/O) module ====================  
 **/
TNCEntry* read_queue(uint64_t page_id);



#endif //TIOCS_TIOCSSD_IO_MANAGER_H