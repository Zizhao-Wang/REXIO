#include "syncstore.h"
#include "../../Backend/SSDWrite/writer.h"
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/IODisk/WriteDisk.h"
#include "../../global_variables/global_variables.h"

int indexs=0;
uint32_t offset = 0;
//TNCEntry * Pagedata = nullptr;
TNCEntry Pagedata[1030];
PageType WBufferId = 0;
int buffernumber =0;
std::unordered_map<uint64_t, std::vector<char>> BufferLog;
LRUCache lrucache(2048);
// FIFOCache fifocache(0);

uint32_t SyncWrite(SKey key1, SValue value)
{

    /* Returned offset */

	if(indexs >= CalculatePageCapacity(sizeof(TNCEntry)))
    {

        SinglePageWrite();
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
    char temp;
    
    uint64_t BlockId = offset>>24;
    //printf("BlockID: %lu\n",BlockId);

    int i =4;
    while(i--)
    {
        temp = (char)(offset & 0XFF);
        offset = offset >> 8;
        BufferLog[BlockId].emplace_back(temp);
    }

    //printf("=========\n");
    if(BufferLog[BlockId].size() >= CalculatePageCapacity(sizeof(char)))
    {
        PageLogWrite(BlockId);
        if(BufferLog[BlockId].size()!=0)
            BufferLog[BlockId].clear();  
    }
    //printf("=========\n");
    return 0;
}

TNCEntry Read4Buffer(size_t pos)
{
    return Pagedata[pos];
}

TNCEntry  SyncRead(uint32_t offset)
{
    uint64_t offsetpage = ((offset>>12)&0xFFF) ;
    uint64_t PageId = (offset>>24)*bp->geo->l.nsectr+ offsetpage;
    size_t Position = (offset & 0x00000FFF)-1;

    if(PageId == WBufferId)
    {
        return Read4Buffer(Position);
    }
    // TNCEntry* ReadData = TNCEntryRead(PageId);
    // TNCEntry a{ReadData[Position].key};
    // delete(ReadData);
    // return a;
    bool IsFlag = lrucache.IsLRUPage(PageId);
    if(!IsFlag)
    {
        //printf("Not Found!,Cache size:%lu\n",lrucache.cache.size());
        buffernumber++;
        ReadNode temp;
        TNCEntry* ReadData = TNCEntryRead(PageId);
        temp.data = ReadData;
        temp.PageId = PageId;
        lrucache.put(PageId, temp);
        TNCEntry tem1{ReadData[Position].key};
        return tem1;    
    }
    else
    {
        //printf("Founded!Cache size:%lu\n",lrucache.cache.size());
        TNCEntry *values = lrucache.get(PageId);
        TNCEntry tem{values[Position].key};
        return tem;
    }
#ifdef LRU
    bool IsFlag = fifocache.IsFIFOPage(PageId);
    if(!IsFlag)
    {
        buffernumber++;
        FReadNode temp;
        TNCEntry* ReadData = TNCEntryRead(PageId);
        temp.data = ReadData;
        temp.PageId = PageId;
        fifocache.put(PageId, temp);
        return ReadData[Position];    
    }
    else
    {
        TNCEntry *values = fifocache.get(PageId);
        return values[Position];
    }
#endif

}