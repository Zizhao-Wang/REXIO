#include "lru.h"

std::unordered_map<uint64_t, ReadNode*> ReadBuffer;




int LRUPut(TNCEntry * data)
{

    return 0;

}

int LRUGet(PageType PageId)
{

    return 0;
}

bool LRUCache::IsLRUPosition(void)
{
	return ReadBuffer.size()<LRUCAPACITY;
}