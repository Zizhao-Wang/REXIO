/**
 * @file nexio_test.h
 * @brief Header file for NEXIO performance testing functions.
 *
 * This file contains declarations of the functions used for performance testing of NEXIO.
 * It includes functions that benchmark various components of the system under different workloads.
 */

#ifndef NEXIO_YCSB_TEST_H
#define NEXIO_YCSB_TEST_H

#include <iostream>
#include <chrono>

void ycsb_update_heavy(uint64_t modified_data_num);


void ycsb_read_heavy(uint64_t modified_data_num);


void ycsb_read_only(uint64_t modified_data_num);


void ycsb_read_latest(size_t operationsCount);


void ycsb_testing(void);


#endif // NEXIO_TEST_H
