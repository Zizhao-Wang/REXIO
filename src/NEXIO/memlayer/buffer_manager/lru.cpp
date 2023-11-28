#include "lru.h"

// key_value_entry* LRUCache::get(page_num_type page)
// {

//     if(!IsLRUPage(page))
//     {
//         return nullptr;
//     }
			
//     ReadNode temp = *HashMap[page];
//     cache.erase(HashMap[page]);
//     cache.push_front(temp);
    
//     HashMap[page] = cache.begin();
//     return temp.data; 

// }

void LRUCache::put(page_num_type page, ReadNode node)
{
    if (!IsLRUPage(page)) 
	{ 
        if (cache.size() == capacity) 
		{
            // cache 已满，删除尾部的键值对腾位置
            // cache 和 map 中的数据都要删除
            auto lastPair = cache.back();
            page_num_type lastKey = lastPair.PageId;
            HashMap.erase(lastKey);
            cache.pop_back();
        }
        // cache 没满，可以直接添加
        cache.push_front(node);
        HashMap[page] = cache.begin();
    }
    else
    {
        cache.erase(HashMap[page]);
        cache.push_front(node);
        HashMap[page] = cache.begin();
    }
     
}

bool LRUCache::IsLRUPage(page_num_type page)
{
    auto it = HashMap.find(page);

    if (it == HashMap.end())
	{
		return false;
	}

    return true; 
}

void LRUCache::ClearaReset(size_t cap)
{
    HashMap.clear();
    cache.clear();

    assert(HashMap.size()==0);
    assert(cache.size()==0);

    this->capacity = cap;
}