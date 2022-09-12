/**
 * @date 2022.4.28 16:32
 * @author zz.wang
 * @details
 *          This file contains some global variables, such as page size, block size and so on.
 *
 *
 *         IDENTIFICATION
 *   Experiment1/AuxizilaryFile/GlobalVariable.h
 */


#ifndef EXPERIMENT1_GLOBALVARIABLE_H
#define EXPERIMENT1_GLOBALVARIABLE_H
#include <cstdint>


/* Global variables are used to LSM-tree. */
extern int LevelNum;

/* The page size is defined as a pre-define variable, because different SSD usually have different page size and block size. */

#define  pagesize   16
#define  blocksize  256
//#define localNodesize 4



typedef uintptr_t Datum;
typedef unsigned int uint_32;


#endif //EXPERIMENT1_GLOBALVARIABLE_H
