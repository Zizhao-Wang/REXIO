/**
 * @date    3/10/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/StoreTier/LRU.h
 */

#ifndef EXPERIMENT1_TNCTREE_LRU_H
#define EXPERIMENT1_TNCTREE_LRU_H

#include <vector>
#include <unordered_map>
#include <list>
#include "../TNCTree/MemoryTier/Node.h"


typedef struct ReadNode
{
	PageType   PageId;
	TNCEntry * data;
}ReadNode;


class LRUCache 
{

private:
    uint16_t capacity;
    std::list<ReadNode> cache;
    // 哈希表：key 映射到 (key, value) 在 cache 中的位置
    std::unordered_map<int, std::list<ReadNode>::iterator> HashMap;

public:
    LRUCache(uint16_t cap) 
	{
        this->capacity = cap; 
    }
    
    TNCEntry* get(PageType page) 
	{

        auto it = HashMap.find(page);
        // 访问的 key 不存在
        if (it == HashMap.end())
		{
			return -1;
		} 
			
        ReadNode temp = *HashMap[page];
        cache.erase(HashMap[page]);
        cache.push_front(temp);
        // 更新 (key, value) 在 cache 中的位置
        HashMap[page] = cache.begin();
        return temp.data; 

    }
    
    void put(int key, int value) 
	{

        /* 要先判断 key 是否已经存在 */ 
        auto it = map.find(key);
        if (it == map.end()) {
            /* key 不存在，判断 cache 是否已满 */ 
            if (cache.size() == cap) {
                // cache 已满，删除尾部的键值对腾位置
                // cache 和 map 中的数据都要删除
                auto lastPair = cache.back();
                int lastKey = lastPair.first;
                map.erase(lastKey);
                cache.pop_back();
            }
            // cache 没满，可以直接添加
            cache.push_front(make_pair(key, value));
            map[key] = cache.begin();
        } else {
            /* key 存在，更改 value 并换到队头 */
            cache.erase(map[key]);
            cache.push_front(make_pair(key, value));
            map[key] = cache.begin();
        }
    }

	bool IsLRUPosition(void);
};



extern std::unordered_map<uint64_t, ReadNode*> ReadBuffer;

int LRUPut(TNCEntry *);

int LRUGet(PageType );



#endif