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
    std::vector<uint64_t>  bucket;
    uint64_t  BucketNo;
    uint64_t  BucketMax;
    bool  IsFirst;

public:
    LBucket();

    /* Insert key into specific vector! */
    void Insert(uint64_t key1);

    /* Erase the bucket but not give up the memory space. */
    void BucketErase();


    /* Return the vector that represents the specific page! */
    std::vector<uint64_t> GetBucket(); 

    /* Return the current size of bucket. */
    size_t GetBucketSize(); 

    uint64_t GetBucketNo();

    bool GetFlag();

    /*
     * Some set() functions is used to set private variables.
     */
    void SetBucketNo(uint64_t bucketno);

};

class LinearHashTable
{
  /* Private variables. */
  private:
    std::vector<LBucket> BucketTable;  // In-memeory buckets table with fixed bucket size.
    uint64_t Tablesize, mod;           // Size of hash table and number is used to operate "mod" 
    const uint64_t BucketBase = 2048;  // maximum of bucket in hash table
    const size_t tablebase = 100;
          
  
  public:
    /* some public function to manipulate private variables. */
    LinearHashTable()
    {
      /*  Constructive function is used to initialize private variables. */
      mod = 100;
      Tablesize = 100;
      for(uint64_t i = 0; i<Tablesize;++i)
      {
        LBucket TempBucket;
        BucketTable.push_back(TempBucket);
      } 
    }

    /* Doubling hash table  */
    int TableDouble()
    {
      for(size_t i=1;i<=tablebase;i++)
      {
        LBucket TempBucket;
        BucketTable.push_back(TempBucket);
      }
      if (BucketTable.size() - Tablesize == tablebase)
      {
        Tablesize = Tablesize + tablebase;
        mod = mod + tablebase;
      }
      else
      {
        printf("Because of unknoen reason, Hash table double failure!\n");
        return -1;
      }
      return 0;

    }

    /* Return 0 if suucess, */
    int split(uint64_t val)
    {
      
      std::vector<uint64_t> TempBucket;  /* Intermediate varible definition */
      if(TableDouble() == -1)
      {
        printf("Doubling hash table failure when spliting bucket %lu because unknown reaon.",val);
        return -1;
        exit(103);
      }
      TempBucket = BucketTable[val].GetBucket();
      BucketTable[val].BucketErase();
      for(int i=0;i<TempBucket.size();i++)
      {
        insert(TempBucket[i],TempBucket[i]);
      }
      return 0;
      
    }

    int insert(uint64_t key, uint64_t value);

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