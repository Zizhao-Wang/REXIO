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
#include "node.h"
#include "../../Backend/IODisk/WriteDisk.h"

/* Global variables declaration */


class LBucket
{
  
private:
    std::vector<LHEntry>  bucket;
    PageType  PageNum, overflowPage;
    uint16_t  BucketMax,size;   //The capacity of a speific bucket
    bool  IsFirst;

public:
    LBucket(uint16_t maxsize);

    /* Insert key into specific vector! */
    int Insert(SKey key1, SValue val);

    void BucketWrite(void);

    /* Erase the bucket but not give up the memory space. */
    
    LHEntry BucketRetrival(SKey key);

    int ValueDelet(SKey);

    void BucketErase();

    /**
     *  "get()" functions is used to set private variables.
     **/
    std::vector<LHEntry> GetBucket(); // Return the vector that represents the specific page! 

    size_t GetBucketSize();  // Return the current size of bucket. 

    PageType GetBucketNo();

    bool GetFlag();

    /*
     *  "set()" functions is used to set private variables.
     */
    void SetBucketNo(PageType bucketno);


    bool IsFull(void) const;

};

class LinearHashTable
{

private:
    std::vector<LBucket> BucketTable;  // In-memeory buckets table with fixed bucket size.
    uint64_t workRound,SplitFlag;               // Size of hash table and number is used to operate "mod"
    size_t h1,h2;       
  
public:
    /* some public function to manipulate private variables. */
    LinearHashTable(uint16_t );

    /* Doubling hash table  */
    int TableDouble();

    /* Return 0 if suucess, */
    int split();

    int Insert(SKey key, SValue value);

    int Search(SKey key);

    int Delete(SValue value);

    int Update(SKey key, SValue val);
};


/* All function declaration!*/
void LHashPort();

#endif //CLASSWORK_HEADER_H