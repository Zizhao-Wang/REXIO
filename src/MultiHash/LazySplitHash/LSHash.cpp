#include "LSHash.h"


LSbucket::LSbucket(uint16_t maxsize)
	:bucket(maxsize)
{
	this->PageNo = UINT64_MAX;
	this->CurrentSize = 0;
}

int LSbucket::Insert(SKey key, SValue value)
{

	return 0;
}

int LSbucket::Insert1(SKey key, SValue value)
{
}


LSHash::LSHash()
{


}

