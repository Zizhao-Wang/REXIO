/**
 * @date 2022.4.28 16:32
 * @author zz.wang
 * @details
 *          This file contains some global variables, such as page size, block size and so on.
 *
 * IDENTIFICATION
 *   src/AuxizilaryFile/GlobalVariable.h
 */


#ifndef EXPERIMENT1_GLOBALVARIABLE_H
#define EXPERIMENT1_GLOBALVARIABLE_H
#include <cstdint>
#include <iostream>

/* Global variables and functions that manipulate global variables in LSM-tree. */
extern long LevelNum;

long LevelAlloctor();


/* The page size is defined as a pre-define variable, because different SSD usually have different page size and block size. */

// #define  pagesize   16
// #define  blocksize  256
// #define localNodesize 4


typedef uintptr_t Datum;
typedef unsigned int uint_32;

/**
 * Globle define key
 **/
typedef uint64_t key;
typedef uint64_t value;

#endif //EXPERIMENT1_GLOBALVARIABLE_H
