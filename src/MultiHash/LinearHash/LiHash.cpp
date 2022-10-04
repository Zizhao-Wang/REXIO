/* @Author: Zizhao Wang
 * @E-mail: zz.wang@siat.ac.cn
 * @Date: 10/8/2022
 * @name: Implementation of Linear-Hashing.
*/

//Some useful header files.

#include "LiHash.h"
#include<iostream>
#include<vector>


/* Global variables definition and utilization, coming soon...  */


LBucket::LBucket()
{
     BucketNo = UINT64_MAX;     
     BucketMax = 2048;      //The capacity of a bucket
}


void LBucket::Insert(uint64_t key1)
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


void LBucket::BucketErase()
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


std::vector<SValue> LBucket::GetBucket()
{
     return bucket;
}


size_t LBucket::GetBucketSize()
{
     return bucket.size();
}


PageType LBucket::GetBucketNo()
{
     return BucketNo;
}

bool LBucket::GetFlag()
{
     return IsFirst;
}


void LBucket::SetBucketNo(uint64_t bucketno)
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
//     clock_t startTime,endTime;                        // Definition of timestamp

//      /* workload a: insert only*/
//      startTime = clock();
//      for(SKey i=1;i<=40000000;i++)
//      {
//           // if(i!=0&&i%100000==0)
//           // printf("Insert %lu successful!\n",i);
//           InsertNode(i,i);
//      }
//      endTime = clock();
//      std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


//      /* workload b: read only, all in it */
//      startTime = clock();
//      for(int i=1;i<=1000000;i++)
//      {
//           srand48(time(NULL));
//           SKey k = 1+(rand()%40000000);
//           Search(k);
//           if(i==10000 || i%100000==0)
//           {
//                endTime = clock();
//                std::cout << "Total Time of "<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
//           }
//      }
//      endTime = clock();
//      printf("BufferNumber: %d",buffernumber);
//      buffernumber = 0;
//      std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

//      // /* workload c: read only, 50% in it, 50% not in it */
//      startTime = clock();
     
//      for(int i=1;i<=1000000;i++)
//      {
//           srand48(time(NULL));
//           if(i%100<50)
//           {
//                SKey k = 1+(rand()%40000000);
//                Search(k);
//           }
//           else
//           {
//                SKey k = 40000000+(rand()%40000000);
//                Search(k);
//           }
//           if(i%100000==0 || i==10000)
//           {
//                endTime = clock();
//                std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
//           }
//      }
//      printf("BufferNumber: %d",buffernumber);
//      endTime = clock();
//      std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

//      /* workload d: update heavy workload, 50% read, 50% update */
//      startTime = clock();
//      for(int i=1;i<=1000000;i++)
//      {
//           srand48(time(NULL));
//           if(i%2==0)
//           {
//                SKey k = 1+(rand()%60000000);
//                Search(k);
//           }
//           else
//           {
//                SKey k = 1+(rand()%60000000);
//                Update(k,k+1);
//           }
//           if(i%100000==0 || i==10000)
//           {
//                endTime = clock();
//                std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
//           } 
//      }
//      endTime = clock();
//      std::cout << "Total Time of workload d: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

//      /* workload E: read mostly workload, 95% read, 5% update */
//      startTime = clock();
//      for(int i=1;i<=1000000;i++)
//      {
//           srand48(time(NULL));
//           if(i%100<95)
//           {
//                SKey k = 1+(rand()%60000000);
//                Search(k);
//           }
//           else
//           {
//                SKey k = 1+(rand()%60000000);
//                Update(k,k+1);
//           }
//           if(i%100000==0 || i==10000)
//           {
//                endTime = clock();
//                std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
//           } 
//      }
//      endTime = clock();
//      std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

//      /* workload F: read latest workload, 95% read, 5% insert */
//      startTime = clock();
//      for(int i=1;i<=1000000;i++)
//      {
//           srand48(time(NULL));
//           if(i%100<95)
//           {
//                SKey k = 1+(rand()%50000000);
//                Search(k);
//           }
//           else
//           {
//                SKey k = 1+(rand()%50000000);
//                InsertNode(i+40000000,i+40000000);
//           }
//           if(i%100000==0 || i==10000)
//           {
//                endTime = clock();
//                std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
//           } 
//      }
//      endTime = clock();
//      std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

//      /* workload G: delete workload, 100% delete*/
//      startTime = clock();
//      for(int i=1;i<=1000000;i++)
//      {
//           srand48(time(NULL));
//           SKey k = 1+(rand()%50000000);
//           Delete(k);
//           if(i%100000==0 || i==10000)
//           {
//                endTime = clock();
//                std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
//           }  
//      }
//      endTime = clock();
//      std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

}
