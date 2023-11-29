/**
 * @file nexio_test.h
 * @brief Header file for NEXIO performance testing functions.
 *
 * This file contains declarations of the functions used for performance testing of NEXIO.
 * It includes functions that benchmark various components of the system under different workloads.
 */

#ifndef NEXIO_TEST_H
#define NEXIO_TEST_H

#include <iostream>
#include <chrono>
#include <gflags/gflags.h>
#include "include/pre_definition.h"

extern int error_bound;
extern uint64_t user_input_bytes;
extern double total_operation_time;
extern uint64_t last_operation_time;
extern double total_latency;
extern double max_latency;
extern int j;
extern double min_latency;
extern double throughput;

extern double throughput2; 

extern double average_latency ;



DECLARE_string(benchmarks);
DECLARE_int64(num);
DECLARE_int32(buckets);
DECLARE_int64(value_size);
DECLARE_string(pci_address);


/**
 * @brief Perform a benchmark test on NEXIO.
 *
 * This function runs a set of operations on NEXIO to evaluate its performance.
 * It measures the time taken to complete a specified number of operations and prints the results.
 */
void bench_testing(void);

#endif // NEXIO_TEST_H
