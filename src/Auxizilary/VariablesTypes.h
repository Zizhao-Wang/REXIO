/**
 * @date   7/9/2022
 * @author zz.wang
 * @details
 *         This file shows how to construct a LSM-Tree.
 *
 * IDENTIFICATION:
 *          code/Auxizilary/VariablesTypes.h
 **/

#ifndef EXPERIMENT1_VARIABLESTYPES_H
#define EXPERIMENT1_VARIABLESTYPES_H

#include <iostream>


typedef uint64_t KEY_t;
typedef uint64_t VAL_t;

#define KEY_MAX 18446744073709551615
#define KEY_MIN 0


#define VAL_MAX 18446744073709551615
#define VAL_MIN 0


#define PAGE_MAX 4294967295
 
typedef struct entry 
{
    KEY_t key;
    VAL_t val;
    bool operator == (const entry& other) const 
    {
        return key == other.key;
    }
    bool operator<(const entry& other) const 
    {
        return key < other.key;
    }
    bool operator>(const entry& other) const 
    {
        return key > other.key;
    }
}entry_t;

#endif  // EXPERIMENT1_VARIABLESTYPES_H