/**
 * @date:   4/8/2022
 * @author: zz.wang
 * @details:
 *          This file contains all fuction declaration and definition of class used in "Lhash.cpp". 
 *
 * IDENTIFICATION:
 *          src/MultipleHash/Bucket.h
 **/

#ifndef EXPERIMENT1_LINEARHASH_LIHash_H
#define EXPERIMENT1_LINEARHASH_LIHash_H

#include <iostream>
#include <vector>

#include "../../Backend/IODisk/WriteDisk.h"

/* Global variables declaration */


class LBucket
{
  
private:
    std::vector<SKey>  bucket;
    PageType  PageNum;
    uint16_t  BucketMax;   //The capacity of a speific bucket
    bool  IsFirst;

public:
    LBucket(uint16_t maxsize);

    /* Insert key into specific vector! */
    void Insert(uint64_t key1);

    /* Erase the bucket but not give up the memory space. */
    void BucketErase();

    /**
     *  "get()" functions is used to set private variables.
     **/
    std::vector<uint64_t> GetBucket(); // Return the vector that represents the specific page! 

    size_t GetBucketSize();  // Return the current size of bucket. 

    uint64_t GetBucketNo();

    bool GetFlag();

    /*
     *  "set()" functions is used to set private variables.
     */
    void SetBucketNo(uint64_t bucketno);

};

class LinearHashTable
{

private:
    std::vector<LBucket> BucketTable;  // In-memeory buckets table with fixed bucket size.
    uint64_t Tablesize;               // Size of hash table and number is used to operate "mod"
    size_t h1,h2;
    size_t SplitFlag;       
  
public:
    /* some public function to manipulate private variables. */
    LinearHashTable(uint16_t );

    /* Doubling hash table  */
    int TableDouble();

    /* Return 0 if suucess, */
    int split(uint64_t val);

    int Insert(SKey key, SValue value);

    int Search(uint64_t key)
    {
      return 0;
    }

    int Delete(uint64_t value)
    {
      return 0;
    }
};


/* All function declaration!*/
void LHashPort();

#endif //CLASSWORK_HEADER_H