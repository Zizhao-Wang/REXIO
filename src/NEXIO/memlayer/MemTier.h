/**
 * @date   27/4/2022 
 * @author zz.wang
 * @details
 *         If this is the program's first run, the program will create a new special hash table in memory.
 *         Otherwise, the process will read the hash table from the solid-state disk (SSD).
 *
 * IDENTIFICATION:
 *         src/FrontEnd/HashTable/HashTable.h
 */
#ifndef EXPERIMENT1_HASHTABLE_H
#define EXPERIMENT1_HASHTABLE_H

#include <stdint.h>
#include <emmintrin.h> // SSE2
#include "include/GlobalVariable.h" 
#include "include/node.h"



/**
 *  ================= Node initialization module ==================== 
 **/
bool LocalGeneration(GlobalHashNode * global, int initialLocalDepth);

bool DoubleHashtable();

uint64_t big_endian2little_endian(const char *big_endian, size_t len);


/**  
 * ================= Insertion module ====================  
 **/

int RandomLevel();

void NodeSplit(size_t bucket_index);

#ifdef NOT_SEPARATE_KV
int InsertNode(const char* hashkey, uint64_t offset, uint8_t flag, int bucket_id);
#elif defined(NOT_SEPARATE_KV_variable)
int InsertNode(const char* hashkey, uint64_t offset, uint8_t flag, int bucket_id);
#elif defined(SEPARATE_KV_FIXED_LOG)
int InsertNode(const char* hashkey, uint64_t offset, uint8_t flag,uint64_t block, int bucket_id);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
int InsertNode(const char* hashkey, uint64_t offset, uint8_t flag,uint64_t block, int bucket_id);
#endif

int InsertNode(const char* hashkey, const char* hashvalue);

inline int compareWithSSE(const char* a, const char* b, size_t size) {
    const size_t sseWidth = 16; // SSE可以一次处理16个字节
    size_t iterations = size / sseWidth;

    for (size_t i = 0; i < iterations; ++i) {
        __m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(a + i * sseWidth));
        __m128i v2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(b + i * sseWidth));

        __m128i result = _mm_cmpeq_epi8(v1, v2); 
        int mask = _mm_movemask_epi8(result);

        if (mask != 0xFFFF) {
            for (size_t j = 0; j < sseWidth; ++j) {
                if (a[i * sseWidth + j] != b[i * sseWidth + j]) {
                    return (a[i * sseWidth + j] < b[i * sseWidth + j]) ? -1 : 1;
                }
            }
        }
    }

    // 处理剩余的字节
    for (size_t i = size - size % sseWidth; i < size; ++i) {
        if (a[i] != b[i]) {
            return (a[i] < b[i]) ? -1 : 1;
        }
    }

    return 0;
}



/**
 *  =================  Search module  ==================== 
 **/

#ifndef FastSkiplist
TSkiplistNode * SearchNode(TNCSkiplist * Head,const char* hashkey);
#endif
#ifdef FastSkiplist
LocalHashNode * SearchNode(LocalHeadNode * Head,const char* hashkey);
#endif

value_type Search(const char* );


/**  
 * ================= update module ====================  
 **/
int Update(const char* key1, const char* val);


/**
 *  ================= deletion module====================  
 **/
#ifndef FastSkiplist
    bool DeleteValue(TNCSkiplist * Head, const char* hashkey);
#endif

#ifdef FastSkiplist
    bool DeleteValue(LocalHeadNode * Head, const char* hashkey);
#endif

int Delete(const char* key1);


/**  
 * ================= Memory-tier initialization module ====================  
 **/
int ExtendHashTableInitialize();


void PrintHashTable();

#endif //EXPERIMENT1_HASHTABLE_H
