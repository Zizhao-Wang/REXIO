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

void LSbucket::AllClear(void)
{

}
int LSbucket::PageWrite()
{

	return 0;


}

LSHash::LSHash()
{




}


int LSHash::Insert(SKey key, SValue value)
{

	return 0;
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





