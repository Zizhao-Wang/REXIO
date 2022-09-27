#include "LSHash.h"


LSbucket::LSbucket(uint16_t maxsize)
	:bucket(maxsize)
{

}
int LSbucket::Insert(SKey key, SValue value)
{

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

