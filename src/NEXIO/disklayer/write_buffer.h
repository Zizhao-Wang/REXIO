/**
 * @date    2/10/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/StoreTier/asnycstore.h
 */

#ifndef NEXIO_DISKLAYER_WRITEBUFFER_H
#define NEXIO_DISKLAYER_WRITEBUFFER_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include "include/node.h"


#define VALUE_BLOCK 1
#define KEY_BLOCK   0
#define SPDK_LBAs_IN_NEXIO_WRITE_BUFFER 128     // page size: 16KB
#define SPDK_LBAs_IN_NEXIO_LBA          1024   //block size: 1M
#define SPDK_LBAs_IN_NEXIO_LOG_BUFFER   16

// ==========================
// Offset and buffer related 
// ==========================
extern uint32_t offset5;
extern uint32_t offset2;

typedef struct write_io_controller
{
    uint64_t buffer_capacity;
    uint64_t nexio_lba_uint;
    uint64_t nexio_write_uint;
    uint64_t current_write_lba_num;
    uint64_t write_buffer_size;
    uint64_t nexio_log_unit;
    uint64_t nexio_log_buffer_size;
}write_io_controller;
extern write_io_controller my_controller;



// ========================
// Separated key-value store
// ========================
extern size_t key_position_in_buffer;
extern char *key_separated_buffer;
extern size_t value_position_in_buffer;
extern char *value_separated_buffer;
extern size_t key_block_id;
extern size_t value_block_id;
extern size_t block_id_allocator;

// ======================
// Combined key-value store
// ======================
extern char *combined_kv_buffer;
extern page_num_type combined_kv_page_num;
extern int current_buffer_position;

// ==========================
// Synchronization primitives
// ==========================
extern pthread_mutex_t write_buffer_mutex;
extern pthread_cond_t buffer_full_cond_var;


// ==========
// Log buffer
// ==========
extern size_t log_block_id_allocator;
extern std::unordered_map<uint64_t, int> block_type_tracker;
extern std::unordered_map<uint64_t, std::vector<char>> log_buffer;

// ==========
// Temp storage
// ==========
extern char *temp_storage;

// ==========
// For read
// ==========
extern uint64_t write_buffer_id;

/**
 * @brief Initializes the write buffer's mutex and condition variable.
 *
 * This function sets up synchronization primitives for the write buffer. 
 * It is essential to call this before using the buffer in a multithreaded environment.
 *
 * @param buffer_size Size of the write buffer.
 */
 void initialize_write_buffer_variables();

/**
 * @brief Initializes the combined key-value buffer using DMA memory.
 *
 * This function allocates memory for the combined key-value buffer using Direct Memory Access (DMA).
 * It is essential to ensure that the buffer is appropriately set up for high-speed I/O operations.
 */
void combined_kv_buffer_init();


/**
 * @brief Cleans up the combined key-value buffer by deallocating its memory.
 *
 * This function releases the memory previously allocated for the combined key-value buffer.
 * It should be called when the buffer is no longer needed, to ensure proper memory management.
 */
void combined_kv_buffer_cleanup();




/**
 * @brief Initializes the separated key-value buffers using DMA memory.
 *
 * This function allocates memory for separated key and value buffers, and the log data buffer.
 * It ensures that each buffer is set up appropriately for high-speed I/O operations.
 * Additionally, if the key and value block IDs haven't been set, they are initialized and tracked.
 */
void kv_buffer_init();

/**
 * @brief Cleans up the separated key-value and log data buffers by deallocating their memory.
 *
 * This function releases the memory previously allocated for the separated key-value buffers and the log data buffer.
 * It should be called when these buffers are no longer needed, ensuring proper memory management.
 */
void kv_buffer_cleanup();





char* read_form_write_Buffer(uint64_t pos);





#endif