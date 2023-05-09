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

extern uint64_t page_pointer;

extern struct spdk_nvme_qpair *qpair;


int write_queue();

TNCEntry* read_queue(uint64_t page_id);

#endif //TIOCS_TIOCSSD_IO_MANAGER_H