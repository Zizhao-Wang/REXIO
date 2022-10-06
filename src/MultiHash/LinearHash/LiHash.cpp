#include "LiHash.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/SSDWrite/writer.h"

/* Global variables definition and utilization, coming soon...  */
int readcount = 0;
int writecount = 0;
int erasecount = 0;

LBucket::LBucket(size_t  maxsize)
{
     PageNum = UINT64_MAX;     
     this->BucketMax = maxsize;   
}


void LBucket::BucketWrite()
{
     sort(bucket.begin(),bucket.end());
     PageNum = SingleBucketWrite(bucket, PageNum);
}

int LBucket::Insert(SKey key1, SValue val)
{
     LHEntry entry{key1,val};
     bucket.emplace_back(entry);
     size++;
     if(bucket.size() >= BucketMax )
     {
          //printf("key1: %lu val: %lu bucket.size():%lu\n",key1,val,bucket.size());
          BucketWrite();
          bucket.clear();
          return 1;
     }

     return 0;
}

void LBucket::BucketErase()
{
     if(bucket.size()!=0)
          bucket.clear();
     size = 0;
}

LHEntry LBucket::BucketRetrival(SKey key)
{
     //printf("PageNum: %lu ",PageNum);
     if(PageNum != UINT64_MAX )
     {
          std::vector<LHEntry> TempEntry = PageRead(PageNum);
          std::vector<LHEntry>::iterator get;
          get = find(TempEntry.begin(),TempEntry.end(), LHEntry{key,0});
          if(get != TempEntry.end())
          {
                    return  *get;  
          } 
     }

     std::vector<LHEntry>::iterator get;
     get = find(bucket.begin(),bucket.end(), LHEntry{key,0});
     if(get != bucket.end())
     {
          return *get;
     }
     
     return LHEntry{0,0};
}

int  LBucket::ValueDelet(SKey key)
{
     std::vector<LHEntry> temp;
     std::vector<LHEntry>::iterator get;
     get = find(bucket.begin(),bucket.end(), LHEntry{key,0});
     if(get != bucket.end())
     {
          bucket.erase(get);
          size--;
          return 0;
     }

     if(PageNum!=UINT64_MAX && size>=BucketMax)
     {
          temp = PageRead(PageNum);
          get = find(temp.begin(),temp.end(), LHEntry{key,0});
          if(get != temp.end())
          {
               temp.erase(get);
               size--;
               return 0;
          }
          for(int i=0;i<temp.size();i++)
               bucket.emplace_back(temp[i]);
          BucketWrite();
          bucket.clear();
     }
     return 0;

}

int LBucket::ValueUpdate(SKey key1, SValue val)
{
     std::vector<LHEntry> temp;
     std::vector<LHEntry>::iterator get;
     get = find(bucket.begin(),bucket.end(), LHEntry{key1,0});
     if(get != bucket.end())
     {
          (*get).val = val;
          return 0;
     }

     if(PageNum!=UINT64_MAX && size>=BucketMax)
     {
          temp = PageRead(PageNum);
          get = find(temp.begin(),temp.end(), LHEntry{key1,0});
          if(get != temp.end())
          {
               (*get).val = val;
          }
          for(int i=0;i<temp.size();i++)
               bucket.emplace_back(temp[i]);
          BucketWrite();
          bucket.clear();
     }
     return 0;
}
     


std::vector<LHEntry> LBucket::GetBucket(void)
{
#ifdef OverflowPage     
     if(PageNum != UINT64_MAX )
     {
          std::vector<LHEntry> entries = PageRead(PageNum);
          for(size_t i=0;i<bucket.size();i++)
          {
               entries.emplace_back(bucket[i]);
          }
          return entries;
     }
#endif
     if(PageNum != UINT64_MAX )
     {
          std::vector<LHEntry> entries = PageRead(PageNum);
          return entries;
     }
     //printf("test! in Get BUCKET\n");
     return bucket;
}

size_t LBucket::GetBucketSize()
{
     return size;
}

PageType LBucket::GetBucketNo()
{
     return PageNum;
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
     if(size >= BucketMax)
     {
          return true;
     }
     //printf("size:%lu bucketMax:%lu\n",size,BucketMax);
     return false;
}



LinearHashTable::LinearHashTable(size_t Initialsize)
{
     /**
      * Constructive function is used to initialize private variables.
      * There are  global  
      **/
     h1 = Initialsize;
     h2 = 2*h1;
     
     memset(SplitFlag,0,sizeof(SplitFlag));
     size_t MaxSize = CalculatePageCapacity(sizeof(LHEntry));
     //size_t MaxSize = 10;
     for(uint16_t i = 0; i<Initialsize;++i)
     {
        LBucket TempBucket(MaxSize);
        BucketTable.push_back(TempBucket);
     }
     printf("\n ================ Index information ================ \
            \n ---- Initialization successful! \
            \n ---- Meta data space allocated successful! \n"); 
}

void LinearHashTable::TableDouble()
{
     size_t MaxSize = CalculatePageCapacity(sizeof(LHEntry));
     for(int i=0;i<h1;i++)
     {
          LBucket TempBucket(MaxSize);
          BucketTable.push_back(TempBucket);
     }
}


int LinearHashTable::split(size_t SplitBucket)
{
     std::vector<LHEntry> OldBucket;  /* Intermediate varible definition */

     OldBucket = BucketTable[SplitBucket].GetBucket();
     BucketTable[SplitBucket].BucketErase();

     for(size_t i=0;i<OldBucket.size();i++)
     {
          size_t bucketno = OldBucket[i].key % h2;
          if(bucketno>=BucketTable.size())
          {
               TableDouble();
          }
          
          BucketTable[bucketno].Insert(OldBucket[i].key,OldBucket[i].val);
     }
     SplitFlag[SplitBucket] = true;

     bool flag = true;
     for(size_t i=0;i<h1;i++)
     {
          if(!SplitFlag[i])
          {
               flag = false;
               break;
          }
     }
     
     if(flag)
     {
          h1 = h2;
          h2 = 2 * h1;
          memset(SplitFlag,0,sizeof(SplitFlag));
     }
     //printf("Split successful!\n");
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
     //printf("Inserted key:%lu Inserted value:%lu ",key,value);
     size_t bucketno = key % h1; 
     //printf("First bucket number:%lu ",bucketno);
     if(SplitFlag[bucketno]){
          bucketno = key % h2;
         // printf("Second bucket number:%lu ",bucketno);
     }
     //printf("h1:%u h2:%u ",h1,h2);
     if(BucketTable[bucketno].IsFull())
     {
          //printf("bucket size:%lu,h1:%lu h2:%lu",BucketTable[bucketno].GetBucketSize(),h1,h2);
          split(bucketno);
          bucketno = key % h2;
     }
     BucketTable[bucketno].Insert(key,value);

    return 0;
}

int LinearHashTable::Search(SKey key)
{
     size_t bucketno = key % h1;
     // //printf("First bucket:%lu ",bucketno);
     if(SplitFlag[bucketno])
     {
          bucketno = key % h2;
          // printf("S bucket:%lu ",bucketno);
     }
     LHEntry entry; 
     entry =  BucketTable[bucketno].BucketRetrival(key);
     //printf("\n");
     //printf("key: %lu entry.key:%lu ",key,entry.key);
     if(key != entry.key)
     {
          printf("key: %lu, entry.key:%lu, Pageno:%lu,h1:%lu h2:%lu test!!!!!!!!\n",key,entry.key,BucketTable[bucketno].GetBucketNo(),h1,h2);
     }
     

     if(entry.key ==0 && entry.val ==0 || entry.key!=key)
     {
          return -1;
     } 
     return 0;
}

int LinearHashTable::Delete(SKey key)
{
     size_t bucketno = key % h1;
     int err = 0;

     if(SplitFlag[bucketno])
     {
          bucketno = key % h2;
     }
     err =  BucketTable[bucketno].ValueDelet(key);

     if(err != 0)
     {
          return -1;
     } 
     return 0;
}

int LinearHashTable::Update(SKey key, SValue val )
{
     int err = 0;
     size_t bucketno = key % h1;
     if(SplitFlag[bucketno])
     {
          bucketno = key % h2;
     }
     err =  BucketTable[bucketno].ValueDelet(key);
     if(err != 0){
          return -1;
     }

     err = BucketTable[bucketno].Insert(key,val);
     if(err != 0){
          return -1;
     }

     return 0;
}

void LHashPort()
{
  
     clock_t startTime,endTime;  // Definition of timestamp
     LinearHashTable hashtable(10);        // initialize a in-memory hash table
     /* Write datum */

     /* workload a: insert only*/
     startTime = clock();
     for(SKey i=1;i<=100000;i++)
     {
          if(i%10000000==0||i==1000000)
          {
               endTime = clock();
               std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
          }
          //printf("Insert %lu successful!\n",i);
          hashtable.Insert(i,i);
     }
     endTime = clock();
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


     /* workload b: read only, all in it */
     startTime = clock();
     for(int i=1;i<=200;i++)
     {
          srand48(time(NULL));
          SKey k = 1+(rand()%100000);
          hashtable.Search(k);
          if(i==10000 || i%100000==0)
          {
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
          }
     }
     endTime = clock();
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload c: read only, 50% in it, 50% not in it */
     // startTime = clock();
     
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%100<50)
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           hashtable.Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 40000000+(rand()%40000000);
     //           hashtable.Search(k);
     //      }
     //      if(i%100000==0 || i==10000)
     //      {
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      }
     // }
     // printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     // endTime = clock();
     // std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload d: update heavy workload, 50% read, 50% update */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%2==0)
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           hashtable.Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           hashtable.Update(k,k+1);
     //      }
     //      if(i%100000==0 || i==10000)
     //      {
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      } 
     // }
     // printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     // endTime = clock();
     // std::cout << "Total Time of workload d: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload E: read mostly workload, 95% read, 5% update */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%100<95)
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           hashtable.Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           hashtable.Update(k,k+1);
     //      }
     //      if(i%100000==0 || i==10000)
     //      {
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      } 
     // }
     // printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     // endTime = clock();
     // std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload F: read latest workload, 95% read, 5% insert */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%100<95)
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           hashtable.Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           hashtable.Insert(i+40000000,i+40000000);
     //      }
     //      if(i%100000==0 || i==10000)
     //      {
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      } 
     // }
     // printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     // endTime = clock();
     // std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload G: delete workload, 100% delete*/
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      SKey k = 1+(rand()%40000000);
     //      hashtable.Delete(k);
     //      if(i%100000==0 || i==10000)
     //      {
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      }  
     // }
     // printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     // endTime = clock();
     // std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

}
