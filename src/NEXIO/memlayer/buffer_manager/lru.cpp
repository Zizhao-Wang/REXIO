#include "lru.h"
#include "include/buffer.h"


char* LRUCache::get(page_num_type page)
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

void LRUCache::put(page_num_type page, ReadNode node)
{
    if (!IsLRUPage(page)) 
	{ 
        if (cache.size() == capacity) 
		{
            auto lastPair = cache.back();
            page_num_type lastKey = lastPair.PageId;
            spdk_dma_free(lastPair.data);  // 先释放节点的内存
            HashMap.erase(lastKey);
            cache.pop_back();
        }
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

 void LRUCache::freeAllBuffers() {
    for (auto &node : cache) {
        spdk_dma_free(node.data); 
    }
    cache.clear();  
    HashMap.clear();  
}