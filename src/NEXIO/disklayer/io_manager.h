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
#include <unordered_map>
#include <vector>
#include <bitset>
#include "include/node.h"
#include "write_buffer.h"
#include "include/log.h"

/**
 *  ================= I/O records====================  
 **/
extern int out_stand;

extern size_t block_id_allocator;

extern size_t key_num_data_page;

extern size_t value_num_data_page;

extern struct spdk_nvme_qpair *qpair;

extern std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkLog;

extern std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkData;

extern std::unordered_map<uint64_t, uint64_t> block_current_write_pointers;

extern std::vector<std::bitset<SPDK_LBAs_IN_NEXIO_LBA>> block_bitmaps; 

extern std::unordered_map<uint64_t, std::pair<int,int>> block_information;

struct IO_context {
    uint64_t io_size;
};


/**
 * Struct representing a record for calculating write amplification.
 * 
 * Write amplification is the ratio of the actual amount of data written 
 * to the storage medium versus the amount of data requested to be written 
 * by the host system.
 * 
 * \param user_written_data Amount of data that the user intends to write.
 * \param actual_written_data Actual amount of data that gets written after factors like garbage collection.
 */
typedef struct wa_record
{
    uint64_t user_written_data;
    uint64_t actual_written_data;
} wa_record;


/**
 * @def NVME_SSD_DATA_WRITE
 * @brief Represents data writes directed to the NVMe SSD.
 * 
 * This macro is used to indicate operations that involve writing 
 * data directly to the NVMe SSD, as opposed to log writes or other types 
 * of writes.
 */
#define NVME_SSD_DATA_WRITE 1

/**
 * @def NVME_SSD_LOG_WRITE
 * @brief Represents log writes directed to the NVMe SSD.
 * 
 * This macro is used to indicate operations that involve writing 
 * log entries or metadata to the NVMe SSD, as opposed to actual data writes.
 */
#define NVME_SSD_LOG_WRITE  0

/**
 * @def NVME_SSD_DATA_KEY_WRITE
 * @brief Represents key-specific data writes to the NVMe SSD.
 * 
 * This macro is used to indicate operations where only the key 
 * portion of a key-value pair is written to the NVMe SSD.
 */
#define NVME_SSD_DATA_KEY_WRITE 2

/**
 * @def NVME_SSD_DATA_VALUE_WRITE
 * @brief Represents value-specific data writes to the NVMe SSD.
 * 
 * This macro is used to indicate operations where only the value 
 * portion of a key-value pair is written to the NVMe SSD.
 */
#define NVME_SSD_DATA_VALUE_WRITE 3


/**
 * @def NVME_SSD_DATA_LOG_WRITE
 * @brief Represents value-specific data writes to the NVMe SSD.
 * 
 * This macro is used to indicate operations where only the value 
 * portion of a key-value pair is written to the NVMe SSD.
 */
#define NVME_SSD_DATA_LOG_WRITE 4


void init_io_manager();

void lba_update(int mode, uint64_t log_lba_pointers);

void lba_update(int mode, uint64_t current_write_pointer,uint64_t block_id, uint64_t log_pointer);


/**
 * ==============================================================================
 *                                DATA WRITE (I/O) MODULE
 * ==============================================================================
 * This module contains functions and logic related to writing data. It provides
 * both basic and detailed interfaces for data writing, catering to different
 * usage scenarios.
 * ==============================================================================
 */
/**
 * Submit a basic write I/O operation without specific details.
 *
 * This version of the write_queue function serves as a basic entry point for write operations
 * where the specifics of the write, like the starting LBA, length, and data payload, are
 * predetermined or set by internal logic.
 *
 * \note This function is meant for scenarios where the exact details of the write operation
 * are abstracted away from the caller.
 *
 * \return 0 if the operation is successfully executed, otherwise returns appropriate error codes.
 */
int write_queue();

/**
 * Submit a detailed write I/O to the storage medium.
 *
 * This version of the write_queue function provides detailed control over the write operation,
 * allowing the caller to specify the starting LBA, length of the write, and the data buffer.
 *
 * \param lba_start Starting LBA to write the data.
 * \param lba_count Length (in sectors) for the write operation.
 * \param write_buffer Virtual address pointer to the data payload.
 *
 * \return 0 if successfully executed, other appropriate error codes on failure:
 * -EINVAL: The request is malformed.
 * -ENOMEM: The request cannot be allocated.
 * -ENXIO: There was an error at the transport or device level.
 */
int write_queue(uint64_t lba_start, uint64_t lba_count, void* write_buffer);

void process_pending_spdk_io();

int kv_write_queue(void *write_buffer, uint64_t block_id,int mode);

/**  
 * ================= LOG WRITE(I/O) module ====================  
 **/
int write_queue(char *write_buffer, uint64_t block_id);


int kv_log_queue(char *write_buffer, uint64_t block_id);

/**  
 * ================= READ(I/O) module ====================  
 **/
char* read_queue(uint64_t page_id);



void printBlockInformation();

#endif //TIOCS_TIOCSSD_IO_MANAGER_H