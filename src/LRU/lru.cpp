#include "lru.h"

std::unordered_map<uint64_t, ReadNode*> ReadBuffer;



TNCEntry* LRUCache::get(PageType page)
{

    if(!IsLRUPage(page))
    {
        return nullptr;
    }
			
    ReadNode temp = *HashMap[page];
    cache.erase(HashMap[page]);
    cache.push_front(temp);
    
    HashMap[page] = cache.begin();
    return temp.data; 

}

void LRUCache::put(PageType page, ReadNode node)
{
    if (!IsLRUPage(page)) 
	{ 
        if (cache.size() == capacity) 
		{
            // cache 已满，删除尾部的键值对腾位置
            // cache 和 map 中的数据都要删除
            auto lastPair = cache.back();
            PageType lastKey = lastPair.PageId;
            HashMap.erase(lastKey);
            cache.pop_back();
        }
        // cache 没满，可以直接添加
        cache.push_front(node);
        HashMap[page] = cache.begin();
    }
    cache.push_front(node);
    HashMap[page] = cache.begin(); 
}

bool LRUCache::IsLRUPosition(void)
{
	return ReadBuffer.size()<capacity;
}

bool LRUCache::IsLRUFull(void)
{
	return ReadBuffer.size()>=capacity;
}

bool LRUCache::IsLRUPage(PageType page)
{
    auto it = HashMap.find(page);

    if (it == HashMap.end())
	{
		return false;
	}

    return true; 
}