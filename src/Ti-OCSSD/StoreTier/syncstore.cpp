#include "syncstore.h"
#include "../../Backend/SSDWrite/writer.h"
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/IODisk/WriteDisk.h"
#include "../../global_variables/global_variables.h"
#include "../MemoryTier/MemTier.h"
#include "io_manager.h"
#include "assistant.h"

int indexs=0;
uint32_t offset = 0;
uint64_t sectors_per_page = 4;
TNCEntry * Pagedata = nullptr;
char* key_buffer =nullptr;
char* value_buffer = nullptr;

size_t key_buffer_position = 0;
size_t value_buffer_position = 0;

size_t key_block_id = UINT64_MAX;

size_t value_block_id =  UINT64_MAX;

size_t buffer_capacity = 0;
size_t buffer_size = 0;
PageType WBufferId = 0;
int buffernumber =0;
std::unordered_map<uint64_t, std::vector<char>> BufferLog;
std::unordered_map<uint64_t, std::vector<unsigned char>> BufferLog2;
LRUCache lrucache(2048);
// FIFOCache fifocache(0);


void buffer_init() 
{
    Pagedata = (TNCEntry*)spdk_dma_malloc(sizeof(TNCEntry) * buffer_capacity, 0x1000, NULL);
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

void kv_buffer_init() 
{
    buffer_size = geometry.ws_min * geometry.clba;
    key_buffer = (char*)spdk_dma_malloc(buffer_size, 0x1000, NULL);
    if (key_buffer == NULL) 
    {
        printf("Failed to allocate memory for Pagedata.\n");
    }

    value_buffer = (char*)spdk_dma_malloc(buffer_size, 0x1000, NULL);
    if (value_buffer == NULL) 
    {
        printf("Failed to allocate memory for Pagedata.\n");
    }

    if(key_block_id == UINT64_MAX)
    {
        key_block_id = block_id_allocator++;
    }

    if(value_block_id == UINT64_MAX)
    {
        value_block_id = block_id_allocator++;
    }
}

void kv_buffer_cleanup() 
{
    spdk_dma_free(key_buffer);
    spdk_dma_free(value_buffer);
}

uint32_t SynckvseparateWrite(const char* hashkey, const char* hashvalue, uint8_t& block)
{

    if (value_buffer_position+VAL_SIZE > buffer_size) 
    {

        kv_write_queue(value_buffer,value_block_id);

        uint64_t pages_point = BlockWritePointers[value_block_id] + value_block_id*geometry.clba;

        WBufferId = pages_point;
        offset += pages_point % geometry.clba == 0 ? 0x1000000 : 0x00000000;

        offset = offset & 0xFF000FFF;
        offset += ((pages_point % geometry.clba) << 12);

        offset = offset & 0xFFFFF000;
        indexs = 0;

        value_buffer_position = 0;
    }

    memcpy(value_buffer + value_buffer_position, hashvalue, VAL_SIZE);
    value_buffer_position += VAL_SIZE;

    offset++;

    
    if (key_buffer_position+KEY_SIZE > buffer_size) 
    {
        kv_write_queue(key_buffer,key_block_id);
        key_buffer_position = 0;
    }

    memcpy(key_buffer + key_buffer_position, hashkey, KEY_SIZE);
    key_buffer_position += KEY_SIZE;

    block = key_block_id;

    return offset;
}


uint32_t SyncvseparateWrite(const char* hashvalue, uint8_t& block)
{

    if (value_buffer_position+VAL_SIZE > buffer_size) 
    {

        kv_write_queue(value_buffer,value_block_id);

        uint64_t pages_point = BlockWritePointers[value_block_id] + value_block_id*geometry.clba;

        WBufferId = pages_point;
        offset += pages_point % geometry.clba == 0 ? 0x1000000 : 0x00000000;

        offset = offset & 0xFF000FFF;
        offset += ((pages_point % geometry.clba) << 12);

        offset = offset & 0xFFFFF000;
        indexs = 0;

        value_buffer_position = 0;
    }

    memcpy(value_buffer + value_buffer_position, hashvalue, VAL_SIZE);
    value_buffer_position += VAL_SIZE;

    offset++;
    return offset;
}

uint32_t SynckseparateWrite(const char* hashkey,uint8_t& block)
{
    if (key_buffer_position+KEY_SIZE > buffer_size) 
    {
        kv_write_queue(key_buffer,key_block_id);
        key_buffer_position = 0;
    }

    memcpy(key_buffer + key_buffer_position, hashkey, KEY_SIZE);
    key_buffer_position += KEY_SIZE;

    block = key_block_id;

    return offset;
}


uint32_t SyncWrite(const char* hashkey, const char* hashvalue)
{
    writes_ram++;
    /* Returned offset */
    // printf("SyncWrite hashkey: %lu\n", big_endian2little_endian(hashkey, KEY_SIZE));
    if (indexs >= buffer_capacity)
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

    ++indexs;
    block_information[offset>>24].first++;
    offset++;

    return offset;
}

int countBits(uint32_t n) 
{
    int count = 0;
    while (n) 
    {
        n >>= 1;
        ++count;
    }
    return count;
}



/**
 *  ================= Synchronous delete module====================  
 **/
int  SynckvDelete(uint32_t offset)
{ 
    
    int position = 0;

    uint64_t BlockId = offset >> 24;
    uint32_t data = offset & 0xFFFFFF;

    uint32_t pageID = data >> 12;  
    uint32_t offsetInPage = data & 0xFFF;  
    data = pageID * (buffer_size/VAL_SIZE) + offsetInPage;  

        
    int length = EncodeLength(data);

    if( BufferLog2[BlockId].size() + length > buffer_size) 
    {
        char* LogDataBuffer = (char *)spdk_dma_malloc(sectors_per_page * geometry.clba, 0x1000, NULL);
        if (LogDataBuffer == NULL)
        {
            printf("Failed to allocate memory for LogDataBuffer!\n");
            exit(-1);
        }
        memcpy(LogDataBuffer, BufferLog[BlockId].data(), BufferLog[BlockId].size());
        write_queue(LogDataBuffer, BlockId);
        BufferLog[BlockId].clear();
        spdk_dma_free(LogDataBuffer);
    }

        
    for (int i = length - 1; i >= 0; i--) 
    {
        unsigned char encoded = data & 0x7F; 
        data >>= 7;  
        if (i == 0) 
        {
            encoded |= 0x80;
        }
        BufferLog2[BlockId].push_back(encoded);
    }
   
}



int SyncDelete(uint32_t offset)
{ 
    resets++;
    char temp;
    
    uint64_t BlockId = offset >> 24;
    // block_information[BlockId].second++;
    
    int i = 4;
    // while (i--)
    // {
    //     temp = (char)(offset & 0XFF);
    //     offset = offset >> 8;
    //     BufferLog[BlockId].emplace_back(temp);
    // }

    // if (BufferLog[BlockId].size() >= (sectors_per_page * geometry.clba ))
    // {
    //     char* LogDataBuffer = (char *)spdk_dma_malloc(sectors_per_page * geometry.clba, 0x1000, NULL);
    //     if (LogDataBuffer == NULL)
    //     {
    //         printf("Failed to allocate memory for LogDataBuffer!\n");
    //         exit(-1);
    //     }
    //     memcpy(LogDataBuffer, BufferLog[BlockId].data(), BufferLog[BlockId].size());
    //     write_queue(LogDataBuffer, BlockId);
    //     BufferLog[BlockId].clear();
    //     spdk_dma_free(LogDataBuffer);
    // }
    return 0;
}



/**
 *  ================= Synchronous read module====================  
 **/
TNCEntry Read4Buffer(size_t pos)
{
    return Pagedata[pos];
}

TNCEntry  SyncRead(uint32_t offset)
{
    uint64_t offsetpage = ((offset>>12)&0xFFF) ;
    uint64_t PageId = (offset>>24)*geometry.clba+ offsetpage;
    size_t Position = (offset & 0x00000FFF)-1;

    // printf("SyncRead offset: %u (Block: %u, Page: %u, Position: %u), PageId: %lu, Position: %zu\n", offset, (offset >> 24), ((offset >> 12) & 0xFFF), (offset & 0x00000FFF), PageId,Position);

    if(PageId == WBufferId)
    {
        return Read4Buffer(Position);
    }

    bool IsFlag = lrucache.IsLRUPage(PageId);
    if(true)
    {
        //printf("Not Found!,Cache size:%lu\n",lrucache.cache.size());
        buffernumber++;
        // ReadNode temp;
        TNCEntry* ReadData = read_queue(PageId);
        // temp.data = ReadData;
        // temp.PageId = PageId;
        // lrucache.put(PageId, temp);
        TNCEntry tem1;
        memcpy(tem1.key,ReadData[Position].key,KEY_SIZE);
        memcpy(tem1.val,ReadData[Position].val,VAL_SIZE);
        spdk_dma_free(ReadData);
#ifdef TIOCS_READ_DEBUG
        printf("ReadData[Position].key: %lu ", big_endian2little_endian(ReadData[Position].key, KEY_SIZE));
        printf("ReadData[Position].val: %lu\n",big_endian2little_endian(ReadData[Position].val, KEY_SIZE));
#endif
        return tem1;    
    }
    else
    {
        // printf("Founded!Cache size:%lu\n",lrucache.cache.size());
        TNCEntry *values = lrucache.get(PageId);
        TNCEntry tem;
        memcpy(tem.key,values[Position].key,KEY_SIZE);
        memcpy(tem.val,values[Position].val,VAL_SIZE);
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