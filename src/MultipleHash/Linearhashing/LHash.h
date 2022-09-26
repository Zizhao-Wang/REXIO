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
    LBucket()
    {
      /*  Initialize some necessary in-class variables */
      BucketNo = UINT64_MAX;     //Bucket number = page number 
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
        //printf("Insert key %lu into bucket %lu successful, size of the bucket is %lu after inserting.\n", key1,BucketNo,nowsize);
      }
      else
      {
        //printf("Because some unknown reasons, insertion failure!\n");
      }
    }

    /* Erase the bucket but not give up the memory space. */
    void BucketErase()
    {
      bucket.clear();
      if (bucket.size() == 0) 
      {
        printf("Bucket %lu clear succeed!\n",BucketNo);
      }
      else
      {
        printf("Bucket %lu clear failure.\n",BucketNo);
      }
    }

    /*
     * Some Get() function is used to get private variables.
     */
    
    std::vector<uint64_t> GetBucket() /* Return the vector that represents the specific page! */
    {
      return bucket;
    }
    
    size_t GetBucketSize() /* Return the current size of bucket. */
    {
      return bucket.size();
    }

    uint64_t GetBucketNo()
    {
      return BucketNo;
    }
    bool GetFlag()
    {
      return IsFirst;
    }

    /*
     * Some set() functions is used to set private variables.
     */
    void SetBucketNo(uint64_t bucketno)
    {
      if(BucketNo == UINT64_MAX)
      {
        BucketNo = bucketno;
      }
      else
      {
        printf("Set bucket number failure, %lu can not be setted!\n",bucketno);
        exit(104);
      }
    }

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