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
#include <cstring>


#define KEY_MAX 18446744073709551615
#define KEY_MIN 0

#define VAL_MAX 18446744073709551615
#define VAL_MIN 0

#define PAGE_MAX 429 967295

#define KEY_SIZE 16
#define VAL_SIZE 128

typedef uint64_t key_type;
typedef uint64_t value_type;

typedef uint64_t page_num_type;


extern int reads_io;  // read I/Os

extern uint64_t total_write_bytes; // write I/Os

extern int resets; 

extern int io_resets; // 

extern int writes_ram; 



typedef struct entry 
{
    char key[KEY_SIZE];
    char val[VAL_SIZE];
    // char timestamp[8];

    bool operator <(const entry& other) const
    {
        int key_comparison_result = memcmp(key, other.key, KEY_SIZE);
        if (key_comparison_result == 0)
        {
            return memcmp(val, other.val, VAL_SIZE) < 0;
        }
        return key_comparison_result < 0;
    }

    bool operator >(const entry& other) const
    {
        int key_comparison_result = memcmp(key, other.key, KEY_SIZE);
        if (key_comparison_result == 0)
        {
            return memcmp(val, other.val, VAL_SIZE) > 0;
        }
        return key_comparison_result > 0;
    }

    bool operator ==(const entry& other) const
    {
        return memcmp(key, other.key, KEY_SIZE) == 0 && memcmp(val, other.val, VAL_SIZE) == 0;
    }

}entry_t;


// gloabl functions
uint64_t big_endian2little_endian(const char *big_endian, size_t len);

void write_data(const std::string& file_path, const std::string& experiment_num, const std::string& workload_num, int count_point);


#endif  // EXPERIMENT1_VARIABLESTYPES_H