#include "syncstore.h"
#include "../../Backend/SSDWrite/writer.h"
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/IODisk/WriteDisk.h"
#include "../../global_variables/global_variables.h"
#include "../MemoryTier/MemTier.h"
#include "io_manager.h"

int indexs=0;
uint32_t offset = 0;
uint64_t sectors_per_page = 8;
TNCEntry * Pagedata = nullptr;
size_t buffer_size = 0;
PageType WBufferId = 0;
int buffernumber =0;
std::unordered_map<uint64_t, std::vector<char>> BufferLog;
LRUCache lrucache(2048);
// FIFOCache fifocache(0);


void buffer_init() 
{
    Pagedata = (TNCEntry*)spdk_dma_malloc(sizeof(TNCEntry) * buffer_size, 0x1000, NULL);
    if (Pagedata == NULL) 
    {
        printf("Failed to allocate memory for Pagedata.\n");
    }
    WBufferId = page_pointer;
}

void buffer_cleanup() 
{
    spdk_dma_free(Pagedata);
}

uint32_t SyncWrite(const char* hashkey, const char* hashvalue)
{
    /* Returned offset */
    // printf("SyncWrite hashkey: %lu\n", big_endian2little_endian(hashkey, KEY_SIZE));
    if (indexs >= buffer_size)
    {
        write_queue();

        WBufferId = page_pointer;
        offset += page_pointer % geometry.clba == 0 ? 0x1000000 : 0x00000000;

        offset = offset & 0xFF000FFF;
        offset += ((page_pointer % 4096) << 12);

        offset = offset & 0xFFFFF000;
        indexs = 0;
    }

    memcpy(Pagedata[indexs].key, hashkey, KEY_SIZE);
    memcpy(Pagedata[indexs].val, hashvalue, VAL_SIZE);
    
    if(big_endian2little_endian(hashkey, KEY_SIZE) == 117334)
    {
        printf("SyncWrite hashkey: %lu\n", big_endian2little_endian(hashkey, KEY_SIZE));
        printf("SyncWrite offset: %u (Block: %u, Page: %u, Position: %u), page_pointer: %lu, indexs: %d\n",
           offset,
           (offset >> 24),
           ((offset >> 12) & 0xFFF),
           (offset & 0x00000FFF),
           page_pointer,
           indexs);
        printf("Pagedata[indexs].key: %lu\n", big_endian2little_endian(Pagedata[indexs].key, KEY_SIZE));
    }

    ++indexs;

    offset++;

    return offset;
}


int  SyncDelete(uint32_t offset)
{ 
    // char temp;
    
    // uint64_t BlockId = offset>>24;
    // //printf("BlockID: %lu\n",BlockId);

    // int i =4;
    // while(i--)
    // {
    //     temp = (char)(offset & 0XFF);
    //     offset = offset >> 8;
    //     BufferLog[BlockId].emplace_back(temp);
    // }

    // //printf("=========\n");
    // if(BufferLog[BlockId].size() >= CalculatePageCapacity(sizeof(char)))
    // {
    //     PageLogWrite(BlockId);
    //     if(BufferLog[BlockId].size()!=0)
    //         BufferLog[BlockId].clear();  
    // }
    // //printf("=========\n");
    // return 0;
}

TNCEntry Read4Buffer(size_t pos)
{
    return Pagedata[pos];
}

TNCEntry  SyncRead(uint32_t offset)
{
    uint64_t offsetpage = ((offset>>12)&0xFFF) ;
    uint64_t PageId = (offset>>24)*geometry.clba+ offsetpage;
    size_t Position = (offset & 0x00000FFF)-1;

    printf("SyncRead offset: %u (Block: %u, Page: %u, Position: %u), PageId: %lu, Position: %zu\n", offset, (offset >> 24), ((offset >> 12) & 0xFFF), (offset & 0x00000FFF), PageId,Position);

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
        // buffernumber++;
        // ReadNode temp;
        TNCEntry* ReadData = read_queue(PageId);
        // temp.data = ReadData;
        // temp.PageId = PageId;
        // lrucache.put(PageId, temp);
        TNCEntry tem1;
        memcpy(tem1.key,ReadData[Position].key,KEY_SIZE);
        memcpy(tem1.val,ReadData[Position].val,VAL_SIZE);
        printf("ReadData[Position].key: %lu ", big_endian2little_endian(ReadData[Position].key, KEY_SIZE));
        printf("ReadData[Position].val: %lu\n",big_endian2little_endian(ReadData[Position].val, KEY_SIZE));
        return tem1;    
    }
    else
    {
        //printf("Founded!Cache size:%lu\n",lrucache.cache.size());
        TNCEntry *values = lrucache.get(PageId);
        TNCEntry tem;
        memcpy(tem.key,values[Position].key,KEY_SIZE);
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