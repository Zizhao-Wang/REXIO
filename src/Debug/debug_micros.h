/**
 * @date   28/4/2022
 * @author zz.wang
 * @details
 *          This file shows how to construct a LSM-Tree.
 *
 * IDENTIFICATION:
 *          src/Debug/Debug.h
 **/

#ifndef TIOCS_DEBUG_H
#define TIOCS_DEBUG_H

/**
 * define whether printf debug information 
 * */
#define DEBUG   



/**
 * Define micros first level of LSM-tree 
 **/
// #define SKIPLIST

#define RED_BLACK_TREE

#define DEFAULT_THREAD_COUNT 2          

// #define TIOCS_DEBUG

// #define BIG_TO_LITTLE_ENDIAN

// #define TIOCS_READ_DEBUG

#define FIXED_LOG



/**
 * Global MICRO for all indexes
 **/
// #define NOT_SEPARATE_KV

// #define NOT_SEPARATE_KV_variable

// #define SEPARATE_KV_VARIABLE_LOG

#define SEPARATE_KV_FIXED_LOG

#define IO_DEBUG

#define DIRECT_ERASE

// #define DELAYED_ERASE



/**
 * NoFTL-KV MICRO
 */
// #define SINGLE_THREAD_IO

// #define MULTI_THREAD_IO
#define SINGLE_THREAD_IO
#endif // TIOCS_DEBUG_H