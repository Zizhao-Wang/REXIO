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


extern int MAX_BUFFER_TASKS;

#include <stdint.h>
#include <iostream>


int CreateMultiProcess();


/**
 * @brief Predicts the next available offset for writing.
 *
 * This function calculates and updates the next suitable offset for data placement 
 * based on the current state and the SSD geometry. It serves as a mechanism to 
 * ensure that data is laid out efficiently on the SSD.
 *
 * @return Predicted offset for the next data write.
 */
uint32_t predict_next_offset();


/**
 * @brief Asynchronously writes key-value pairs.
 *
 * This function writes the provided key-value pairs into the write buffer in 
 * an asynchronous manner. If the buffer is full, a signal is sent to indicate 
 * that the buffer is ready for I/O operations.
 *
 * @param hashkey   The hash key to write.
 * @param hashvalue The hash value corresponding to the key.
 * @return Predicted offset where the key-value pair would be written on the disk.
 */
uint32_t async_write(const char* hashkey, const char* hashvalue);







#endif

