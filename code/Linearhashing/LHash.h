#ifndef EXPERIMENT1_HEADER_H
#define EXPERIMENT1_HEADER_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>


void LHashPort();

/*
 * Some pre-define variables 
 */
#define BucketSize 100  // size of a bucket
#define Tablebase  100  // In-memory table size


class LBucket
{
  public:
    std::vector<uint64_t>  bucket;
    uint64_t  BucketNo;  
};

class LinearHashTable
{
  /*
   * Private variables. 
   */
  private:
    std::vector<LBucket> BucketTable;  //In-memeory buckets table with fixed bucket size.

    uint64_t Tablesize;
    uint64_t mod;
    // vector<set<int>> Overflow;

  public:
  /*
   * some public function to manipulate private variables. 
   */
    LinearHashTable(int sz)
    {
      /*
       * Constructive function is used to initialize private variables. 
       */
      Tablesize = Tablebase;
      for(int i = 0; i<Tablesize;i++)
      {
        LBucket TempBucket;
        BucketTable.push_back(TempBucket);
      }
    }

    // void split(int val)
    // {
    //   LBucket.push_back();
    //   for(auto v:Bucket[val])
    //   {
    //     if(v%(2*mod)==val+mod)
    //     {
    //       Bucket[val+mod].insert(v);
    //     }  
    //   }
        
    //   for(auto v:Bucket[val+mod])
    //   {
    //     Bucket[val].erase(v);
    //   }
        
    //   for(auto v:Overflow[val])
    //   if(v%(2*mod)==val+mod)  Overflow[val+mod].insert(v);
    //   for(auto v:Overflow[val+mod]) Overflow[val].erase(v);
    // }

    // void changemod()
    // {
    //   for(int i=0;i<mod;i++) Bucket.PB(set<int>()),Overflow.PB(set<int>());
    //   mod*=2;marker=0;
    // }

    int insert(int value)
    {

      /*
       * This function is implemented to insert a special value into a bucket according to
       * a special rule that usually is called “Linear Hashing method”. 
       * The following source code contains three steps:
       *  1. Find a proper bucket
       *  2. Insert this value into this in-memory bucket 
       *  4. Synchronize the value with Disk
       *  3. update in-memory table 
       */

      /*
       * step 1. Find a proper bucket for a specific value 
       */
      // int bucketno = value % mod;  
      // if(Buckets[bucketno].size() >= BucketSize)
      // {
      //   bucketno = value % (mod*2);
      // }
      // if(Buckets[bucketno].count(x)+ Overflow[bucketno].count(x))
      // {
      //   return 0;
      // } 
      // if(Bucket[bucketno].size()==bufsize) 
      // {
      //   Overflow[bucketno].insert(x);
      //   split(marker++);
      //   if(marker==mod) changemod();
      // }
      // else Bucket[bucketno].insert(x);
      return 1;
    }
};



// class HashTable:public Hash 
// {

//     int max_col;
//     // vector<string> _vec;

// public:

//    /* 
//     * Here are some useful functions for updating hash table:
//     * 1. Inserting a new key/value data pair.
//     * 2. Deleting a Key/Value item.
//     * 3. Searching one or more specific value entries. 
//     */

// };

// /* Declaration of L-Hashing functions */
// void LinearHashing(); // port of L-Hashing



#endif //CLASSWORK_HEADER_H