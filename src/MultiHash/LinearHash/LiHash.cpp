#include "LiHash.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/SSDWrite/writer.h"

/* Global variables definition and utilization, coming soon...  */


LBucket::LBucket(uint16_t maxsize)
{
     PageNum = UINT64_MAX;     
     this->BucketMax = maxsize;   
}


void LBucket::BucketWrite(void)
{
     PageNum = SingleBucketWrite(bucket, PageNum);
}

int LBucket::Insert(SKey key1, SValue val)
{
     
     LHEntry entry{key1,val};
     bucket.emplace_back(entry);
     size = bucket.size();
     if(bucket.size() >= BucketMax)
     {
          BucketWrite();
          bucket.clear();
          return 1;
     }

     return 0;
}

void LBucket::BucketErase()
{
     bucket.clear();
     AssertCondition(bucket.size() == 0);
}

LHEntry LBucket::BucketRetrival(SKey key)
{
     if(bucket.size()<CalculatePageCapacity(sizeof(LHEntry)))
     {
          std::vector<LHEntry>::iterator get;
          get = find(bucket.begin(),bucket.end(), LHEntry{key,0});
          if(get != bucket.end())
          {
               return  *get;
          }
          return LHEntry{0,0};
     }
     AssertCondition(PageNum != UINT64_MAX);
     std::vector<LHEntry> TempEntry = PageRead(PageNum);

     std::vector<LHEntry>::iterator get;
     get = find(bucket.begin(),bucket.end(), new LHEntry{key,0});
     if(get != bucket.end())
     {
          return *get;
     }

     return LHEntry{0,0};

}

std::vector<LHEntry> LBucket::GetBucket()
{
     if(PageNum != UINT64_MAX)
     {
          std::vector<LHEntry> entries = PageRead(PageNum);
          for(size_t i=0;i<bucket.size();i++)
          {
               entries.emplace_back(bucket[i]);
          }
          return entries;
     }

     return bucket;
}

size_t LBucket::GetBucketSize()
{
     return bucket.size();
}

PageType LBucket::GetBucketNo()
{
     return PageNum;
}

bool LBucket::GetFlag()
{
     return IsFirst;
}

void LBucket::SetBucketNo(uint64_t bucketno)
{
     if(PageNum == UINT64_MAX){
        PageNum = bucketno;
     }
     else{
          EMessageOutput("Set bucket number failure, %lu can not be setted!\n",104);
     }
}

bool LBucket::IsFull(void) const
{
     if(size<BucketMax)
     {
          return true;
     }
     return false;
}



LinearHashTable::LinearHashTable(uint16_t Initialsize)
{
     /**
      * Constructive function is used to initialize private variables.
      * There are  global  
      **/
     h1 = Initialsize;
     h2 = 2*h1;
     
     SplitFlag = 0;
     
     workRound = 1;

     size_t MaxSize = CalculatePageCapacity(sizeof(LHEntry));
     for(uint16_t i = 0; i<Initialsize;++i)
     {
        LBucket TempBucket(MaxSize);
        BucketTable.push_back(TempBucket);
     } 
}



int LinearHashTable::split()
{
      
     std::vector<LHEntry> OldBucket;  /* Intermediate varible definition */
     LBucket TempBucket(CalculatePageCapacity(sizeof(LHEntry)));
     BucketTable.emplace_back(TempBucket);

     OldBucket = 


     TempBucket = BucketTable[val].GetBucket();
     BucketTable[val].BucketErase();
     for(int i=0;i<TempBucket.size();i++)
     {
          insert(,TempBucket[i]);
     }

     return 0;

}

int LinearHashTable::Insert(SKey key, SValue value)
{
     /** 
      * This function is implemented to insert a special value into a bucket according to
      * a special rule that usually is called “Linear Hashing method”. 
      * The following source code contains three steps:
      *  1. Find a proper bucket
      *  2. Insert this value into this in-memory bucket 
      *  4. Synchronize the value with Disk
      *  3. update in-memory table 
      **/
     int err = 0;
     size_t bucketno = key % h1; 

     if(bucketno < SplitFlag){
          bucketno = key % h2;
     }

     if(BucketTable[bucketno].IsFull())
     {
          split();
     }
     else
     {
          BucketTable[bucketno].Insert(key,value);
     }
    return 0;
}

int LinearHashTable::Search(SKey key)
{
     size_t bucketno = key % h1;
     if(bucketno < SplitFlag)
     {
          bucketno = key % h2;
     }
     LHEntry entry; 
     entry =  BucketTable[bucketno].BucketRetrival(SKey);
     if(entry.key ==0 && entry.val ==0 || entry.key!=key)
     {
          return -1;
     } 
     return 0;
}

int LinearHashTable::Delete(SKey key)
{
     size_t bucketno = key % h1;
     if(bucketno < SplitFlag)
     {
          bucketno = key % h2;
     }
     LHEntry entry; 
     entry =  BucketTable[bucketno].BucketRetrival(SKey);
     if(entry.key ==0 && entry.val ==0 || entry.key!=key)
     {
          return -1;
     } 
     return 0;
}

void LHashPort()
{
  
     clock_t startTime,endTime;  // Definition of timestamp
     LinearHashTable hashtable(100);        // initialize a in-memory hash table
     /* Write datum */

     /* workload a: insert only*/
     startTime = clock();
     for(SKey i=1;i<=40000000;i++)
     {
          // if(i!=0&&i%100000==0)
          // printf("Insert %lu successful!\n",i);
          hashtable.insert(i,i);
     }
     endTime = clock();
     std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


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
