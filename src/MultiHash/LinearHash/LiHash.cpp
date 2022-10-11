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
     this->size = 0;   
}


void LBucket::BucketWrite()
{
     sort(bucket.begin(),bucket.end());
     PageNum = SingleBucketWrite(bucket, PageNum);
     bucket.clear();
}

int LBucket::Insert(SKey key1, SValue val)
{
     LHEntry entry{key1,val};
     bucket.emplace_back(entry);
     size++;
     if(size == CalculatePageCapacity(sizeof(LHEntry)))
     {
          BucketWrite();
     }

     return 0;
}

void LBucket::BucketErase()
{
     assert(bucket.size()==0);
     size = 0;
}

void LBucket::BucketErase(bool flag )
{
     if(true)
     {
          bucket.clear();
          size = 0;
          PageNum = UINT64_MAX;
     }
     
}

LHEntry LBucket::BucketRetrival(SKey key)
{
     //printf("Key:%lu bucketsize: %lu",key,bucket.size());
     if(bucket.size()!=0)
     {
          std::vector<LHEntry>::iterator get;
          get = find(bucket.begin(),bucket.end(), LHEntry{key,0});
          if(get != bucket.end())
          {
               return *get;
          }
          return LHEntry{0,0};
     }
     // for(int i=0;i<bucket.size();i++)
     // {
     //      printf("%lu in bucket",bucket[i].key);
     // }
     // printf("============\n");
     // printf("PageNum: %lu ",PageNum);
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
     return LHEntry{0,0};
}

int  LBucket::ValueDelet(SKey key)
{
     if(bucket.size()!=0)
     {
          std::vector<LHEntry>::iterator get;
          get = find(bucket.begin(),bucket.end(), LHEntry{key,0});
          if(get != bucket.end())
          {
               bucket.erase(get);
               size--;
               return 0;
          }
     }
     else
     {
          bucket = PageRead(PageNum);
          std::vector<LHEntry>::iterator get;
          size += BucketMax;
          get = find(bucket.begin(),bucket.end(), LHEntry{key,0});
          if(get != bucket.end())
          {
               bucket.erase(get);
               size--;
               return 0;
          }
     }

     return 1;
}

int LBucket::ValueUpdate(SKey key1, SValue val)
{ 
     if(bucket.size()!=0)
     {
          std::vector<LHEntry>::iterator get;
          get = find(bucket.begin(),bucket.end(), LHEntry{key1,0});
          if(get!=bucket.end())
          {
               (*get).val = val;
                return 0;
          }
     }
     else
     {
          bucket = PageRead(PageNum);
          std::vector<LHEntry>::iterator get;
          get = find(bucket.begin(),bucket.end(), LHEntry{key1,0});
          if(get != bucket.end())
          {
               (*get).val = val;
          }
          BucketWrite();
          return 0;
     }
     return -1;
}
     


std::vector<LHEntry> LBucket::GetBucket(void)
{
     if(size < CalculatePageCapacity(sizeof(LHEntry)))
     {
          return bucket;
     }
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
     return bucket;
}

std::vector<LHEntry> LBucket::GetBucketRead(void)
{

     assert(size == CalculatePageCapacity(sizeof(LHEntry)));
     if(PageNum != UINT64_MAX && bucket.size()==0)
     {
          std::vector<LHEntry> entries = PageRead(PageNum);
          return entries;
     }
     //printf("test! in Get BUCKET\n");
     return bucket;
}


size_t LBucket::GetSize()
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
     if(size>=BucketMax )
     {
          return true;
     }
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

void LinearHashTable::TableDisplay()
{
     for(int i=0;i<BucketTable.size();i++)
     {
          printf("bucket size:%lu No. bucket:%lu memory size:%lu\n",BucketTable[i].GetSize(),BucketTable[i].GetBucketNo(),BucketTable[i].bucket.size());

          for(int i = 0;i<h1;i++)
          {
               printf("i:%d ",SplitFlag[i]);
          }
          printf("bucket Max:%lu IsFull():%d \n",BucketTable[i].BucketMax,BucketTable[i].IsFull());
          // if(BucketTable[i].GetBucketNo()!=UINT64_MAX)
          //      PageReadTest(BucketTable[i].GetBucketNo());
     }
     printf("\n\n\n");
}


void LinearHashTable::TableDouble()
{
     size_t MaxSize = CalculatePageCapacity(sizeof(LHEntry));
     for(int i=0;i<h1;i++)
     {
          LBucket TempBucket(MaxSize);
          BucketTable.emplace_back(TempBucket);
     }
}


int LinearHashTable::split(size_t SplitBucket)
{

     //TableDisplay();
     
     std::vector<LHEntry> OldBucket;  /* Intermediate varible definition */

     OldBucket = BucketTable[SplitBucket].GetBucketRead();
     BucketTable[SplitBucket].BucketErase();
     SplitFlag[SplitBucket] = true;

     for(size_t i=0;i<OldBucket.size();i++)
     {
          size_t bucketno = OldBucket[i].key % h2;
          if(bucketno>=BucketTable.size())
          {
               TableDouble();
          }
          
          BucketTable[bucketno].Insert(OldBucket[i].key,OldBucket[i].val);
     }
     

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
          h1 = 2* h1;
          h2 = 2* h2;
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
     //printf("SplitFlag[bucketno]:%d\n ",SplitFlag[bucketno]);
     if(SplitFlag[bucketno])
     {
          bucketno = key % h2;
          // printf("S bucket:%lu ",bucketno);
     }
     LHEntry entry; 
     entry =  BucketTable[bucketno].BucketRetrival(key);
     
     // if(key != entry.key)
     // {
     //      //printf("key: %lu, entry.key:%lu, Pageno:%lu,h1:%lu h2:%lu bucketsize:%lu test!!!!!!!!\n",key,entry.key,BucketTable[bucketno].GetBucketNo(),h1,h2);
     //      printf("key:%lu Not Found!\n",key);
     // }
     

     if(entry.key ==0 && entry.val ==0 || entry.key!=key)
     {
          return -1;
     } 
     return 0;
}

bool LinearHashTable::Judge(size_t bucketno)
{
     size_t b2;
     if(bucketno <h1)
          b2 = bucketno+h1;
     else
          b2 = bucketno-h1;
     return BucketTable[bucketno].GetSize()+BucketTable[b2].GetSize() <= CalculatePageCapacity(sizeof(LHEntry));
     
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

     if(SplitFlag[bucketno])
     {
          if(Judge(bucketno))
          {
               size_t b1,b2;
               if(bucketno < h1)
               {
                    b1 = bucketno;
                    b2 = b1+h1;
               }
               else
               {
                    b1 = bucketno -h1;
                    b2 =bucketno;
               }
               SplitFlag[b1] = false;
               std:vector<LHEntry> data = BucketTable[b2].GetBucket();
               BucketTable[b2].BucketErase(true);
               for(auto a:data)
               {
                    BucketTable[b1].Insert(a.key,a.val);
               }
          }
          
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
     err =  BucketTable[bucketno].ValueUpdate(key,val);
     if(err != 0){
          printf("test2  ");
          exit(0);
          return -1;
     }

     return 0;
}

void LHashPort()
{
  
     clock_t startTime,endTime;  // Definition of timestamp
     LinearHashTable hashtable(4);        // initialize a in-memory hash table
     /* Write datum */

     /* workload a: insert only*/
     printf(("=========workload A=====\n"));
     startTime = clock();
     for(SKey i=1;i<=40000000;i++)
     {
          if(i%10000000==0||i==1000000)
          {
               endTime = clock();
               std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
          }
          hashtable.Insert(i,i);
     }
     endTime = clock();
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
     //hashtable.TableDisplay();


     /* workload b: read only, all in it */
     startTime = clock();
     printf(("=========workload B=====\n"));
     for(int i=1;i<=1000000;i++)
     {
          srand48(time(NULL));
          SKey k = 1+(rand()%40000000);
          hashtable.Search(k);
          if(i==10000 || i%100000==0)
          {
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
               printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);     
          }
     }
     endTime = clock();
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     /* workload c: read only, 50% in it, 50% not in it */
     startTime = clock();
     printf(("=========workload C=====\n"));
     readcount=0;
     writecount=0;
     erasecount=0;
     
     for(int i=1;i<=1000000;i++)
     {
          srand48(time(NULL));
          if(i%100<50)
          {
               SKey k = 1+(rand()%40000000);
               hashtable.Search(k);
          }
          else
          {
               SKey k = 40000000+(rand()%40000000);
               hashtable.Search(k);
          }
          if(i%100000==0 || i==10000)
          {
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";  
                   printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);   
          }
     }
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     endTime = clock();
     std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     /* workload d: update heavy workload, 50% read, 50% update */
     startTime = clock();
     printf(("=========workload D=====\n"));
     readcount=0;
     writecount=0;
     erasecount=0;
     for(int i=1;i<=1000000;i++)
     {
          srand48(time(NULL));
          if(i%2==0)
          {
               SKey k = 1+(rand()%40000000);
               hashtable.Search(k);
          }
          else
          {
               SKey k = 1+(rand()%40000000);
               hashtable.Update(k,k+1);
          }
          if(i%100000==0 || i==10000)
          {
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
               printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);     
          } 
     }
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     endTime = clock();
     std::cout << "Total Time of workload d: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     /* workload E: read mostly workload, 95% read, 5% update */
     readcount=0;
     writecount=0;
     erasecount=0;
     startTime = clock();
     printf(("=========workload E=====\n"));
     for(int i=1;i<=1000000;i++)
     {
          srand48(time(NULL));
          if(i%100<95)
          {
               SKey k = 1+(rand()%40000000);
               hashtable.Search(k);
          }
          else
          {
               SKey k = 1+(rand()%40000000);
               hashtable.Update(k,k+1);
          }
          if(i%100000==0 || i==10000)
          {
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n"; 
               printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);    
          } 
     }
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     endTime = clock();
     std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     /* workload F: read latest workload, 95% read, 5% insert */
     startTime = clock();
     printf(("=========workload F=====\n"));
     readcount=0;
     writecount=0;
     erasecount=0;
     for(int i=1;i<=1000000;i++)
     {
          srand48(time(NULL));
          if(i%100<95)
          {
               SKey k = 1+(rand()%40000000);
               hashtable.Search(k);
          }
          else
          {
               SKey k = 1+(rand()%40000000);
               hashtable.Insert(i+40000000,i+40000000);
          }
          if(i%100000==0 || i==10000)
          {
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
               printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);    
          } 
     }
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     endTime = clock();
     std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     /* workload G: delete workload, 100% delete*/
     startTime = clock();
     printf(("=========workload G=====\n"));
     readcount=0;
     writecount=0;
     erasecount=0;
     for(int i=1;i<=1000000;i++)
     {
          srand48(time(NULL));
          SKey k = 1+(rand()%40000000);
          hashtable.Delete(k);
          if(i%100000==0 || i==10000)
          {
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
          }  
     }
     printf("Read count:%d Write count:%u Erase Count:%d \n",readcount,writecount,erasecount);
     endTime = clock();
     std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

}
