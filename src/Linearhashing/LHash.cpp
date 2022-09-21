/* @Author: Zizhao Wang
 * @E-mail: zz.wang@siat.ac.cn
 * @Date: 10/8/2022
 * @name: Implementation of Linear-Hashing.
*/

//Some useful header files.

#include<iostream>
#include<vector>
#include "./LHash.h"

/* Global variables definition and utilization, coming soon...  */

int LinearHashTable::insert(uint64_t key, uint64_t value)
{
  /* This function is implemented to insert a special value into a bucket according to
      * a special rule that usually is called “Linear Hashing method”. 
      * The following source code contains three steps:
      *  1. Find a proper bucket
      *  2. Insert this value into this in-memory bucket 
      *  4. Synchronize the value with Disk
      *  3. update in-memory table 
      */

    uint64_t bucketno = key % mod; 
    //printf("Parameter testing: mod:%lu bucketno:%lu key:%lu value:%lu\n",mod,bucketno,key,value); 
    if(BucketTable[bucketno].GetBucketSize() >= BucketBase)
    {
      //printf("Split Parameter testing: mod:%lu bucketno:%lu key:%lu value:%lu\n",mod,bucketno,key,value); 
      int err = 0;
      uint64_t pageno;

      err = split(bucketno);
      if(err == -1)
      {
        printf("Bucket spliting failure!\n");
      }
      BucketTable[bucketno].Insert(key);
      //printf("Test: Pageno %lu\n",BucketTable[bucketno].GetBucketNo());
      if(BucketTable[bucketno].GetBucketNo() == UINT64_MAX)
      {
        pageno = SingleValueWrite(value,BucketTable[bucketno].GetBucketNo(),BucketTable[bucketno].GetBucketSize()-1);
        BucketTable[bucketno].SetBucketNo(pageno);
      }
      else
      {
        pageno = SingleValueWrite(value,BucketTable[bucketno].GetBucketNo(),BucketTable[bucketno].GetBucketSize()-1);
        if(pageno != BucketTable[bucketno].GetBucketNo())
        {
          printf("Fatal error in spliting: Bucket number don't matching!\n");
          return -1;
        }
      }
    }
    else
    {
        uint64_t pageno;
        
        //printf("Unsplit Parameter testing: mod:%lu bucketno:%lu key:%lu value:%lu\n",mod,bucketno,key,value);
        BucketTable[bucketno].Insert(key);
        // //printf("Value:%lu; Pageno %lu; Current size:%lu; bool flag: %lu\n",value,BucketTable[bucketno].GetBucketNo(),BucketTable[bucketno].GetBucketSize(),BucketTable[bucketno].GetFlag());
        if(BucketTable[bucketno].GetBucketNo() == UINT64_MAX)
        {
          
          pageno = SingleValueWrite(value,BucketTable[bucketno].GetBucketNo(),BucketTable[bucketno].GetBucketSize()-1);
          BucketTable[bucketno].SetBucketNo(pageno);
        }
        else
        {
          pageno = SingleValueWrite(value,BucketTable[bucketno].GetBucketNo(),BucketTable[bucketno].GetBucketSize()-1);
          if(pageno != BucketTable[bucketno].GetBucketNo())
          {
            printf("Fatal error: Bucket number don't matching!\n");
            return -1;
          }
        }
    }

    return 1;
}




void LHashPort()
{
  
    clock_t startTime,endTime;  // Definition of timestamp
    LinearHashTable hashtable;        // initialize a in-memory hash table
    /* Write datum */
    startTime = clock();
    for(uint64_t i=1;i<=10000000;i++)
    {
      if(i>=10000 && i%10000 ==0)
      {
        printf("Value:%lu \n",i);
        endTime = clock();
        std::cout << "Total Time of inserting: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
      } 
      uint64_t value = i;
      hashtable.insert(i,value);
    }
    endTime = clock();
    std::cout << "Total Time of inserting: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


    /* Read datum */
    startTime = clock();
    for(int i=1;i<=1000000;i++)
    {
      uint64_t value = i;
      // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of reading data: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    /* data update */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
        uint64_t value = i;
        // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of datum update: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

}
