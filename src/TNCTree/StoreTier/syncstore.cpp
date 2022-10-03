#include "syncstore.h"
#include "../../Backend/SSDWrite/writer.h"
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/IODisk/WriteDisk.h"

int indexs=0;
uint32_t offset = 0;
//TNCEntry * Pagedata = nullptr;
TNCEntry Pagedata[1030];
PageType WBufferId = 0;
std::unordered_map<uint64_t, std::vector<char>> BufferLog;
LRUCache lrucache(32);

uint32_t SyncWrite(SKey key1, SValue value)
{

    /* Returned offset */

	if(indexs >= CalculatePageCapacity(sizeof(TNCEntry)))
    {
        SinglePageWrite();
        offset += sectorpointer%4096==0 ?0x400000:0x00000000 ;
        offset += 0x1000; 
        offset = offset & 0xFFFFF000;
        indexs = 0;
        WBufferId += 4;
    } 
	Pagedata[indexs].key = key1;
	Pagedata[indexs].val = value;
	++indexs;

    ++offset;

    return offset;
}

int  SyncDelete(uint32_t offset)
{ 
    uint8_t temp = 0; 
    uint64_t BlockId = offset>>22;

    while(offset)
    {
        temp = (uint8_t)(offset & 0XFF);
        offset = offset >> 8;
        BufferLog[BlockId].emplace_back(temp);
    }
    

    if(BufferLog[BlockId].size() == CalculatePageCapacity(4))
    {
        PageLogWrite(BlockId);
        BufferLog.clear();    
    }

    return 0;
}

TNCEntry Read4Buffer(size_t pos)
{
    return Pagedata[pos];
}

TNCEntry  SyncRead(uint32_t offset)
{
    uint64_t PageId = (offset>>22)*bp->geo->l.nsectr+((offset>>12)&0x3FF)*4;
    size_t Position = (offset & 0x00000FFF)-1;

    if(PageId == WBufferId)
    {
        return Read4Buffer(Position);
    }

    bool IsFlag = lrucache.IsLRUPage(PageId);
    if(!IsFlag)
    {
        ReadNode temp;
        TNCEntry* ReadData = TNCEntryRead(PageId);
        temp.data = ReadData;
        lrucache.put(PageId, temp);
        return ReadData[Position];    
    }
    else
    {
        TNCEntry *values = lrucache.get(PageId);
        return values[Position];
    }

}