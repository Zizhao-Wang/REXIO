#include "syncstore.h"
#include "../../Backend/SSDWrite/writer.h"
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/IODisk/WriteDisk.h"

int indexs=0;
uint32_t offset = 0;
//TNCEntry * Pagedata = nullptr;
TNCEntry Pagedata[1030];
PageType WBufferId = 0;
int buffernumber =0;
std::unordered_map<uint64_t, std::vector<char>> BufferLog;
LRUCache lrucache(128);

uint32_t SyncWrite(SKey key1, SValue value)
{

    /* Returned offset */

	if(indexs >= CalculatePageCapacity(sizeof(TNCEntry)))
    {

        //SinglePageWrite();
        WBufferId = DataPagePointer;
        offset += DataPagePointer%4096==0?0x1000000:0x00000000;

        offset = offset & 0xFF000FFF; 
        offset += ((DataPagePointer%4096)<<12);

        offset = offset & 0xFFFFF000;
        indexs = 0;

    }

	Pagedata[indexs].key = key1;
	Pagedata[indexs].val = value;
	++indexs;
    
    offset++;

    return offset;
}

int  SyncDelete(uint32_t offset)
{ 
    uint8_t temp = 0;
    
    uint64_t BlockId = offset>>24;
    //printf("offset: %u BlockID: %lu\n",offset,BlockId);


    while(offset)
    {
        temp = (uint8_t)(offset & 0XFF);
        offset = offset >> 8;
        BufferLog[BlockId].emplace_back(temp);
    }
    

    if(BufferLog[BlockId].size() == CalculatePageCapacity(sizeof(uint32_t)))
    {
        PageLogWrite(BlockId);
        BufferLog[BlockId].clear();    
    }

    return 0;
}

TNCEntry Read4Buffer(size_t pos)
{
    return Pagedata[pos];
}

TNCEntry  SyncRead(uint32_t offset)
{
    uint64_t offsetpage = ((offset>>12)&0x3FF) ;
    uint64_t PageId = (offset>>24)*bp->geo->l.nsectr+ offsetpage;
    size_t Position = (offset & 0x00000FFF)-1;

    if(PageId == WBufferId)
    {
        return Read4Buffer(Position);
    }

    bool IsFlag = lrucache.IsLRUPage(PageId);
    if(!IsFlag)
    {
        buffernumber++;
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