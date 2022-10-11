#include "LSHash.h"
#include <cmath>
#include "../../Backend/SSDRead/reader.h"

LSbucket::LSbucket(uint16_t max)
{
  this->maxsize = max;
  this->size = 0;
  PageNum = UINT64_MAX;
}

int LSbucket::Insert(SKey key, SValue value)
{

	LSEntry entry;
	entry.key1 = key;
	entry.val  = value;
  entry.flag = 0;

	BucketEntries.emplace_back(entry);
	size++;

  if(size == maxsize)
  {
    PageWrite();
  }
	return 0;
}

SEntry LSbucket::Retrieval(SKey key)
{

	SEntry entry;

	return entry;

}

int LSbucket::Delete(SKey key)
{
  if(!Insert(key, DELETEVALUE))
  {
    return -1;
  }
	return 0;
}

int LSbucket::Update(SKey key, SValue value)
{


	return 0;
}

bool LSbucket::IsFull(void) const
{
	return size == maxsize;
}

bool LSbucket::IsEmpty(void) const
{
	return size == 0;
}

uint16_t LSbucket::GetMaxSize() const
{
  return maxsize;
}

std::vector<LSEntry> LSbucket::Getdata() 
{
  return BucketEntries;
}

void LSbucket::AllClear(void)
{
  BucketEntries.clear();
  AssertCondition(BucketEntries.size()==0);
}

void LSbucket::Erase(void)
{

  size = 0;
  BucketEntries.clear();
  
}

int LSbucket::PageWrite()
{
  
  return 0;
}

std::vector<LSEntry> LSbucket::BDataRead()
{
  std::vector<LSEntry> reads;
  reads = LSBucketFromPage(PageNum);
  return reads;
}


LSHash::LSHash( uint16_t tablebsize = DEFAULT_TABLEBASESIZE, double ifth = DEFAULT_IFTHRESHOLD)
{

    uint16_t bucketmsize = CalculatePageCapacity(sizeof(LSEntry));
    for(uint16_t i = 0;i<tablebsize;i++)
    {
        LSbucket temp(bucketmsize);
        bucketList.emplace_back(temp);
    }
    this->bmaxsize = bucketmsize;
    this->Allsize = 0;
    this->IFthreshold = ifth;
    //printf("bucketmsize:%d tablebsize:%d IFthreshold:%lf\n",bucketmsize, tablebsize,IFthreshold);

}

SKey LSHash::BitHashfunc(SKey Number, uint8_t bits)
{
  SKey BitNumber;
  BitNumber = Number & (1<<bits)-1;
  //printf("BitNumber:%lu \n",BitNumber);
  return BitNumber;
}

uint8_t LSHash::GetBits(SKey Number)
{
  uint8_t bits = 0;
  //printf("number:%lu ",Number);
  while(Number)
  {
    Number = Number>>1;
    bits++;
  }
  //printf("bits:%d ",bits);
  return bits;
}

bool LSHash::IsNecessary()
{
  return true;
}


void LSHash::Split(size_t BucketNum)
{
  std::vector<LSEntry> tempdata;
  tempdata = bucketList[BucketNum].Getdata();
  bucketList[BucketNum].AllClear();
  
  for(size_t i =0;i<tempdata.size();i++)
  {
    if(Insert(tempdata[i].key1,tempdata[i].val)!=0)
    {
      EMessageOutput("Split failure in data re-allocating in LSHash!",754);
    }
  }

  bucketList[BucketNum].AllClear();
  AssertCondition(bucketList.size()==0);
}

size_t LSHash::SplitNumber()
{
  size_t f = bucketList.size() - pow(2,GetBits(bucketList.size())-1);

  return f;

}

void LSHash::Spliting()
{
  bool split = IsNecessary();

  if
  (split)
  {
    size_t tmp = 0;
    SplitCursor = bucketList.size() - 1;

    while (tmp < SplitNumber())
    {
      Split(tmp);
    }
  }  
}

double LSHash::IFCompute()
{
	return Allsize/(bucketList.size()*bmaxsize);
}

int LSHash::Insert(SKey key, SValue value)
{
    this->Allsize++;
    uint8_t i = GetBits(bucketList.size());
    if(IFCompute()>IFthreshold)
    {
        LSbucket temp(bmaxsize);
        bucketList.emplace_back(temp);
        //split
        uint8_t i = GetBits(bucketList.size());
        size_t num = BitHashfunc(bucketList.size()-1,GetBits(bucketList.size()-1)-1);
        Split(num);
    }
    uint64_t tmp = BitHashfunc(key,i);
    if(tmp >= bucketList.size())
    {
        tmp = BitHashfunc(BitHashfunc(key,i),GetBits(BitHashfunc(key,i))-1);
    }
    bucketList[tmp].Insert(key,value);
	return 0;

}

SEntry LSHash::Retrieval(SKey key)
{
  SEntry entry;
  std::vector<LSEntry> data;

  uint8_t i = GetBits(bucketList.size());
  SKey tmp = BitHashfunc(key,i);

  if(tmp >= bucketList.size())
  {
    tmp = BitHashfunc(BitHashfunc(key,i), GetBits(BitHashfunc(key,i))-1);
  }

  while (tmp > SplitCursor)
  {
    data = bucketList[tmp].BDataRead();
    tmp = BitHashfunc(BitHashfunc(key,i),GetBits(tmp)-1);
  }
  data = bucketList[tmp].BDataRead();
  
  entry = bucketList[tmp].Retrieval(key);

  return entry;

}
int LSHash::Delete(SKey key)
{
  int err = 0;
  SEntry entry = Retrieval(key);
  if(entry.val == DELETEVALUE)
  {
    return 0;
  }

  if(Insert(entry.key1, DELETEVALUE) == 0)
  {
    return 0;
  }

  return -1;

}

int LSHash::Update(SKey key, SValue value)
{

  uint8_t i = GetBits(bucketList.size());
  SKey tmp = BitHashfunc(key,i);

  if(tmp >= bucketList.size())
  {
    tmp = BitHashfunc(BitHashfunc(key,i), GetBits(BitHashfunc(key,i))-1);
  }
  
  if(bucketList[tmp].Update(key,value) == 0)
  {
    return 0;
  }

  return -1;
}





void LSHashPort()
{
  
    clock_t startTime,endTime;  // Definition of timestamp
    LSHash LShashtable;        // initialize a in-memory hash table
    // for(int i=1;i<=1024;i*=2)
    // {
    //   LShashtable.BitHashfunc(i,LShashtable.GetBits(i));
    // }
    // exit(0);

    /* Write datum */
    startTime = clock();
    for(uint64_t i=1;i<=10000000;i++)
    {
      if(i==1000000|| i%10000000 ==0)
      {
        printf("Value:%lu \n",i);
        endTime = clock();
        std::cout << "Total Time of inserting: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
      } 
      uint64_t value = i;
      LShashtable.Insert(i,value);
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





