#include "FIFO.h"
#include<cassert>


TNCEntry* FIFOCache::get(PageType page)
{

    if(!IsFIFOPage(page))
    {
        return nullptr;
    }
			
    FReadNode temp = *HashMap[page];
    
    return temp.data; 

}

void FIFOCache::put(PageType page, FReadNode node)
{
    if (!IsFIFOPage(page)) 
	{ 
        if (Rcache.size() == capacity) 
		{
            // cache 已满，删除尾部的键值对腾位置
            // cache 和 map 中的数据都要删除
            auto lastPair = Rcache.back();
            PageType lastKey = lastPair.PageId;
            HashMap.erase(lastKey);
            Rcache.pop_back();
        }
        // cache 没满，可以直接添加
        Rcache.push_front(node);
        HashMap[page] = Rcache.begin();
    }
    Rcache.push_front(node);
    HashMap[page] = Rcache.begin(); 
}

bool FIFOCache::IsFIFOPage(PageType page)
{
    auto it = HashMap.find(page);

    if (it == HashMap.end())
	{
		return false;
	}

    return true; 
}

void FIFOCache::Clear(size_t cap)
{
    HashMap.clear();

    while (!Rcache.empty())
    {
       Rcache.pop_back();
    }

    assert(Rcache.size()==0);
    assert(HashMap.size()==0);    
    this->capacity =cap;
}