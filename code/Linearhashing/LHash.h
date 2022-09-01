/* @author          zz.wang
 * @date            4/8/2022
 * @note            This is header file of "Lhash.cpp".
 * @description     This file contains all fuction declaration and definition of class used in "Lhash.cpp". 
 * @idetification   code/Linearhashing/LHash.h
 */
#ifndef EXPERIMENT1_HEADER_H
#define EXPERIMENT1_HEADER_H

#include <iostream>
#include <vector>
#include "../Backend/IODisk/WriteDisk.h"

/* Global variables declaration */
extern uint64_t BucketAllocator;


uint64_t AllocatorUse();


class LBucket
{
  private:
    std::vector<uint64_t>  bucket;
    uint64_t  BucketNo;
    uint64_t  BucketMax;


  public:
    LBucket(uint64_t BucketAllocated)
    {
      if(BucketAllocated == -1)
      {
        printf("Unknow reasons! Bucket allocator failure!\n");
        exit(102);
      }
      //printf("Allocated Number %lu\n",BucketAllocated);
      /*  Initialize some necessary in-class variables */
      BucketNo = BucketAllocated;     //Bucket number = page number 
      BucketMax = 2048;              //The capacity of a bucket 
    }

    /* Insert key into specific vector! */
    void Insert(uint64_t key1)
    {
      size_t cursize = bucket.size();
      bucket.push_back(key1);
      size_t nowsize = bucket.size();
      if (nowsize - cursize > 0)
      {
        printf("Insert key %lu into bucket %lu successful, size of the bucket is %lu after inserting.\n", key1,BucketNo,nowsize);
      }
      else
      {
        printf("Because some unknown reasons, insertion failure!\n");
      }
    }

    /* Erase the bucket but not give up the memory space. */
    void BucketErase()
    {

      bucket.clear();
      if (bucket.size() == 0) 
      {
        printf("Successful removed!");
      }
      else
      {
        printf("Clear failure");
      }
      
    }

    /* Return the vector that represents the specific page! */
    std::vector<uint64_t> GetBucket()
    {

      return bucket;

    }

    /* Return the current size of bucket. */
    size_t GetBucketSize()
    {
      return bucket.size();
    }

};

class LinearHashTable
{
  /* Private variables. */
  private:
    std::vector<LBucket> BucketTable;  //In-memeory buckets table with fixed bucket size.
    uint64_t Tablesize, mod;
    const uint64_t BucketBase = 2048;
    const size_t tablebase = 100;
  
  public:


    /* some public function to manipulate private variables. */
    LinearHashTable()
    {
      /*  Constructive function is used to initialize private variables. */
      mod = 100;
      Tablesize = 100;
      for(int i = 0; i<Tablesize;i++)
      {
        LBucket TempBucket(AllocatorUse());
        BucketTable.push_back(TempBucket);
      } 
    }

    /* Doubling hash table  */
    int TableDouble()
    {

      for(size_t i=1;i<=tablebase;i++)
      {
        LBucket TempBucket(AllocatorUse());
        BucketTable.push_back(TempBucket);
      }
      if (BucketTable.size() - Tablesize == tablebase)
      {
        Tablesize += tablebase;
      }
      else
      {
        printf("Because of unknoen reason, Hash table double failure!\n");
        return -1;
      }
      
      return 0;

    }

    /* Return 0 if suucess, */
    int split(int val)
    {
      std::vector<uint64_t> TempBucket;
      LBucket NewBucket(AllocatorUse());

      TempBucket = BucketTable[val].GetBucket();
      for(int i=0;i<TempBucket.size();i++)
      {
        int err = 0;
        err = insert(TempBucket[i],TempBucket[i]);
      }

      return 0;
      
    }

    int insert(uint64_t key, uint64_t value)
    {

     /* This function is implemented to insert a special value into a bucket according to
      * a special rule that usually is called “Linear Hashing method”. 
      * The following source code contains three steps:
      *  1. Find a proper bucket
      *  2. Insert this value into this in-memory bucket 
      *  4. Synchronize the value with Disk
      *  3. update in-memory table 
      */

      int bucketno = key % mod;  
      if(BucketTable[bucketno].GetBucketSize() >= BucketBase)
      {
        int err = 0;
        mod = mod *2;
        err = split(bucketno);
        bucketno = value % mod;
        BucketTable[bucketno].Insert(key);
        //SingleValueWrite(value,bucketno,BucketTable[bucketno].GetBucketSize());
      }
      else
      {
        BucketTable[bucketno].Insert(key);
        //SingleValueWrite(value,bucketno,BucketTable[bucketno].GetBucketSize());
      }
      return 1;
    }

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