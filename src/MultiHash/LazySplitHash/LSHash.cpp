#include "LSHash.h"


LSbucket::LSbucket(uint16_t maxsize)
	:bucket(maxsize)
{

}

int LSbucket::Insert(SKey key, SValue value)
{
	SEntry entry;
	entry.key1 = key;
	entry.val  = value;

	BucketEntries.emplace_back(entry);
	CurrentSize++;

	
	return 0;
}
SValue LSbucket::Retrieval(SKey key)
{
	SValue val;

	return val;

}
int LSbucket::Delete(SKey key)
{

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

void LSbucket::AllClear(void)
{

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

double LSHash::IFCompute()
{
	return Allsize/(bucketList.size()*bmaxsize);
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





