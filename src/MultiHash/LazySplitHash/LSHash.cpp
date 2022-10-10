#include "LSHash.h"


LSbucket::LSbucket(uint16_t max)
{
  this->maxsize = max;
  this->size = 0;
}

int LSbucket::Insert(SKey key, SValue value)
{

	LSEntry entry;
	entry.key1 = key;
	entry.val  = value;
  entry.flag = 0;

	BucketEntries.emplace_back(entry);
	size++;

  if(CurrentSize == MaxSize)
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
	return CurrentSize == MaxSize;
}

bool LSbucket::IsEmpty(void) const
{
	return CurrentSize == 0;
}

uint16_t LSbucket::GetMaxSize() const
{
  return MaxSize;
}

std::vector<SEntry> LSbucket::Getdata() const
{
  return BucketEntries;
}

void LSbucket::AllClear(void)
{
  BucketEntries.clear();
  AssertCondition(BucketEntries.size()==0);
}



int LSbucket::PageWrite()
{
  
  return 0;
}


LSHash::LSHash(uint16_t bucketmsize = DEFAULT_BUCKETMAXSIZE, uint16_t tablebsize = DEFAULT_TABLEBASESIZE, double ifth = DEFAULT_IFTHRESHOLD)
{
    for(uint16_t i = 0;i<tablebsize;i++)
    {
        LSbucket temp(bucketmsize);
        bucketList.emplace_back(temp);
    }
  this->bmaxsize = bucketmsize;
  this->Allsize = 0;
  this->IFthreshold = ifth;
}

SKey LSHash::BitHashfunc(SKey Number, uint8_t bits)
{
  SKey BitNumber;
  BitNumber = Number & (SKey)(1<<bits);
  return BitNumber;
}

uint8_t LSHash::GetBits(SKey Number)
{
  uint8_t bits = 0;
  while(Number)
  {
    Number = Number>>1;
    bits++;
  }

  return bits;
}


void LSHash::Split(size_t BucketNo)
{
  std::vector<SEntry> tempdata;
  tempdata = bucketList[BucketNo].Getdata();
  
  for(size_t i =0;i<tempdata.size();i++)
  {
    if(Insert(tempdata[i].key1,tempdata[i].val)!=0)
    {
      EMessageOutput("Split failure in data re-allocating in LSHash!",754);
    }
  }

  bucketList[BucketNo].AllClear();
  AssertCondition(bucketList.size()==0);

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
    BitHashfunc(bucketList.size()-1,GetBits(bucketList.size()-1)-1);
  }
  uint64_t tmp = BitHashfunc(key,GetBits(bucketList.size()-1)-1);
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
  uint8_t i = GetBits(bucketList.size());
  SKey tmp = BitHashfunc(key,i);

  if(tmp >= bucketList.size())
  {
    tmp = BitHashfunc(BitHashfunc(key,i), GetBits(BitHashfunc(key,i))-1);
  }

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





