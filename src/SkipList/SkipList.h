/**
 * @date:   20/9/2022
 * @author: zz.wang
 * @details:
 *          This is a header file for constructing a in-memory buffer(that equals Level 0 in LSM-Tree).
 *          There are two private variables in "buffer":
 *          std::set<entry_t> Entries and uint64_t MaxSize. 
 *          we constructed Entries for storing data passed from memory. 
 *          With key-values pairs insertig, different size of the set will be recorded. 
 *          The size must be less than or equal to a threshold called "MaxSzie".
 *
 * IDENTIFICATION:
 *          src/SkipList/SkipList.h
 **/

#ifndef EXPERIMENT1_SKIPLIST_H
#define EXPERIMENT1_SKIPLIST_H 



#endif  // EXPERIMENT1_SKIPLIST_H