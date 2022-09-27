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



/* The page size is defined as a pre-define variable, because different SSD usually have different page size and block size. */

// #define  pagesize   16
// #define  blocksize  256
// #define localNodesize 4


typedef uintptr_t Datum;
typedef unsigned int uint_32;

/**
 * "macro" is used to declarative the type of key-value pair with all indexes.
 * (e.g., LSM-Tree, Extendible hash, Linear hash, Lazy-split hash, Two-tier index)
 * 
 * 
 **/
typedef uint64_t SKey;
typedef uint64_t SValue;

typedef struct SEntry
{
	SKey key1;
	SValue val;
}SEntry;


#endif //EXPERIMENT1_GLOBALVARIABLE_H
