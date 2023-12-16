/**
 * @date    2/10/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/StoreTier/asnycstore.h
 */

#ifndef EXPERIMENT1_TNCTREE_ASYNCSTORE_H
#define EXPERIMENT1_TNCTREE_ASYNCSTORE_H



#include <stdint.h>
#include <iostream>
#include "include/node.h"
#include "buffer_manager/lru.h"


extern uint32_t buffernumber;

extern LRUCache lrucache;

// ==========================
// WRITING OPERATIONS
// ==========================
/**
 * @defgroup WritingOperations Writing Operations
 * @{
 */

/**
 * @brief Allocates a task buffer for key-value entries.
 * 
 * This function is responsible for allocating and initializing a buffer
 * that will be used for storing key-value entries.
 * 
 * @return A pointer to the allocated key_value_entry structure.
 */
char* allocate_task_buffer();

/**
 * @brief Asynchronously writes key-value pairs.
 * 
 * This function writes the provided key-value pairs into the write buffer
 * in an asynchronous manner. If the buffer is full, a signal is sent to
 * indicate that the buffer is ready for I/O operations.
 * 
 * @param hashkey The hash key to write.
 * @param hashvalue The hash value corresponding to the key.
 * 
 * @return Predicted offset where the key-value pair would be written on the disk.
 */
uint64_t async_kv_write(const char* hashkey, const char* hashvalue);

/**
 * @brief Asynchronously writes key-value pairs with separate storage consideration.
 * 
 * This function writes the provided key-value pairs into the storage system
 * asynchronously. It considers separating the key and value storage based on
 * certain criteria or optimizations. If necessary, it provides the block or
 * segment identifier where the key-value is stored.
 * 
 * @param hashkey The hash key to write.
 * @param hashvalue The hash value corresponding to the key.
 * @param block Output parameter to indicate the block or segment where
 *        the key-value pair is stored.
 * 
 * @return Predicted offset where the key-value pair would be written on the disk.
 */
uint64_t async_kv_separate_write(const char* hashkey, const char* hashvalue, uint64_t& block);

/**
 * @brief Asynchronously writes key-value pairs with separate and variable length storage.
 * 
 * This function is an extension of async_kv_separate_write, handling variable
 * length values.
 * 
 * @param hashkey The hash key to write.
 * @param hashvalue The hash value corresponding to the key.
 * @param block Output parameter to indicate the block or segment where
 *        the key-value pair is stored.
 * 
 * @return Predicted offset where the key-value pair would be written on the disk.
 */
uint64_t async_kv_separate_variable_write(const char* hashkey, const char* hashvalue, uint64_t& block);

/** @} */ // End of WritingOperations group


// ==========================
// UPDATING OPERATIONS
// ==========================
/**
 * @defgroup UpdatingOperations Updating Operations
 * @{
 */

/**
 * @brief Asynchronously updates a key-value pair with a separate storage strategy.
 * 
 * This function allows for updating the value associated with a specified block
 * in an asynchronous manner, leveraging separate storage mechanisms for keys and values.
 * 
 * @param new_hashvalue The new value to be associated with the key.
 * @param key_offset The offset where the key is stored.
 * 
 * @return The updated offset within the disk.
 */
uint64_t async_kv_separate_update(const char* new_hashvalue, uint64_t key_offset);

/**
 * @brief Asynchronously updates a key-value pair with separate and variable length storage.
 * 
 * This function is an extension of async_kv_separate_update, handling variable
 * length values.
 * 
 * @param new_hashvalue The new value to be associated with the key.
 * @param block The block number where the value should be updated.
 * 
 * @return The updated offset within the disk.
 */
uint64_t async_kv_separate_variable_update(const char* new_hashvalue, uint64_t& block);

/** @} */ // End of UpdatingOperations group


// ==========================
// DELETING OPERATIONS
// ==========================
/**
 * @defgroup DeletingOperations Deleting Operations
 * @{
 */

/**
 * @brief Asynchronously deletes a key-value pair with a variable length value.
 * 
 * @param offset The offset where the key-value pair is located.
 * 
 * @return Integer indicating success (0) or failure (-1).
 */
int async_kv_separte_variable_delete(uint64_t offset);

/**
 * @brief Asynchronously deletes a separate key-value pair.
 * 
 * @param offset The offset where the separate key-value pair is located.
 * 
 * @return Integer indicating success (0) or failure (-1).
 */
int async_kv_separate_delete(uint32_t offset);

/**
 * @brief Asynchronously deletes a key-value pair with a variable length value.
 * 
 * @param offset The offset where the key-value pair with variable length value is located.
 * 
 * @return Integer indicating success (0) or failure (-1).
 */
int async_kv_variable_Delete(uint32_t offset);

/**
 * @brief Asynchronously deletes a key-value pair.
 * 
 * @param offset The offset where the key-value pair is located.
 * 
 * @return 64-bit integer indicating




// ==========================
// READING OPERATIONS
// ==========================
/**
 * @defgroup ReadingOperations Reading Operations
 * @{
 */

/**
 * @brief Asynchronously reads a key-value pair from the storage.
 * 
 * This function initiates an asynchronous read operation to retrieve a key-value
 * pair from the storage based on the provided offset. The result is returned in
 * a key_value_entry structure.
 * 
 * @param offset The offset where the key-value pair is stored.
 * 
 * @return A key_value_entry structure containing the retrieved key-value pair.
 */
char* async_read(uint64_t offset);

/** @} */ // End of ReadingOperations group








#endif

