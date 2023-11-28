#include "syncstore.h"
#include <iostream>
#include <fstream>
#include "memlayer/MemTier.h"
#include "include/buffer.h"
#include "include/spdk_env_init.h"
#include "utility/types.h"

int indexs=0;
int callCount = 0;

size_t buffer_size = 0;

int buffernumber =0;
std::unordered_map<uint64_t, std::vector<char>> BufferLog;
std::unordered_map<uint64_t, std::vector<unsigned char>> BufferLog2;
LRUCache lrucache(2048);
// FIFOCache fifocache(0);

/**
 *  ================= buffer manager module====================  
 **/








/**
 *  ================= Synchronous write module====================  
 **/

uint32_t SyncWrite(const char* hashkey, const char* hashvalue)
{
    // writes_ram++;
    // /* Returned offset */
    // // printf("SyncWrite hashkey: %lu\n", big_endian2little_endian(hashkey, KEY_SIZE));
    // if (indexs >= buffer_capacity)
    // {
    //     write_queue();

    //     WBufferId = page_pointer;
    //     offset5 += page_pointer % geometry.clba == 0 ? 0x1000000 : 0x00000000;

    //     offset5 = offset5 & 0xFF000FFF;
    //     offset5 += ((page_pointer % 4096/4) << 12);

    //     offset5 = offset5 & 0xFFFFF000;
    //     indexs = 0;
    // }

    // memcpy(Pagedata[indexs].key, hashkey, KEY_SIZE);
    // memcpy(Pagedata[indexs].val, hashvalue, VAL_SIZE);

    // ++indexs;
    // block_information[offset5>>24].first++;
    // offset5++;

    // return offset5;
}

uint32_t SynckvseparateWrite(const char* hashkey, const char* hashvalue, uint32_t& block)
{
//     writes_ram++;
//     if (value_buffer_position+VAL_SIZE > buffer_size) 
//     {
//         uint64_t previous_block = value_block_id;
//         kv_write_queue(value_buffer,value_block_id,VALUE_OCSDD_DATA_WRITE);

//         uint64_t pages_point = BlockWritePointers[previous_block] + previous_block*geometry.clba;
//         uint64_t page = BlockWritePointers[previous_block];

//         // printf("key:%lu previous_block: %lu, pages_point: %lu, page: %lu\n",big_endian2little_endian(hashkey,KEY_SIZE),previous_block, pages_point, page);
//         WBufferId = pages_point;
//         // offset += pages_point == num_data_page*sectors_per_page? 0x1000000 : 0x00000000;
//         if (page == num_data_page * sectors_per_page) 
//         {
//             offset5 = (offset5 & 0x00FFFFFF) | (value_block_id << 24);
//             // printf("SynckvseparateWrite offset: %lu,offse:%d \n", big_endian2little_endian(hashkey,KEY_SIZE),offset5>>24);
//         } 

//         offset5 = offset5 & 0xFF000FFF;
//         offset5 += ((page/4) << 12);

//         offset5 = offset5 & 0xFFFFF000;

//         value_buffer_position = 0;
//     }

//     memcpy(value_buffer + value_buffer_position, hashvalue, VAL_SIZE);
//     value_buffer_position += VAL_SIZE;

//     offset5++;
//     block_information[offset5>>24].first++;

//     if(key_buffer_position+KEY_SIZE+sizeof(uint32_t) > buffer_size) 
//     {
//         uint64_t previous_block = key_block_id;
//         kv_write_queue(key_buffer,key_block_id,KEY_OCSDD_DATA_WRITE);
//         uint64_t pages_point = BlockWritePointers[previous_block] + previous_block*geometry.clba;
//         uint64_t page = BlockWritePointers[previous_block];
//         WBufferId = pages_point;
//         if (page == num_data_page * sectors_per_page) 
//         {
//             offset2 = (offset2 & 0x00FFFFFF) | (key_block_id << 24);
//         }

//         offset2 = offset2 & 0xFF000FFF;
//         offset2 += ((page/4) << 12);

//         offset2 = offset2 & 0xFFFFF000;
//         key_buffer_position = 0;
//     }

//     memcpy(key_buffer + key_buffer_position, hashkey, KEY_SIZE);
//     key_buffer_position += KEY_SIZE;

//     memcpy(key_buffer + key_buffer_position, &offset5, sizeof(uint32_t));
//     key_buffer_position += sizeof(uint32_t);

//     offset2++;
//     block = offset2;
//     block_information[offset2>>24].first++;
// #ifdef IO_DEBUG
//     // if(big_endian2little_endian(hashkey,KEY_SIZE) == 8456562)
//     // {
//     //     printf("offset5: %u,offset2:%u\n", offset5,offset2);
//     // }
// #endif
//     return offset5;
}


uint32_t SynckvvariableseparateWrite(const char* hashkey, const char* hashvalue, uint32_t& block)
{

//     if (value_buffer_position+VAL_SIZE > buffer_size) 
//     {
//         uint64_t previous_block = value_block_id;
//         kv_write_queue(value_buffer,value_block_id,VALUE_OCSDD_DATA_WRITE);
//         uint64_t pages_point = BlockWritePointers[previous_block] + previous_block*geometry.clba;
//         uint64_t page = BlockWritePointers[previous_block];
//         // printf("key:%lu previous_block: %lu, pages_point: %lu, page: %lu\n",big_endian2little_endian(hashkey,KEY_SIZE),previous_block, pages_point, page);
//         WBufferId = pages_point;
//         // offset += pages_point == num_data_page*sectors_per_page? 0x1000000 : 0x00000000;
//         if (page == num_data_page * sectors_per_page) 
//         {
//             offset5 = (offset5 & 0x00FFFFFF) | (value_block_id << 24);
//             // printf("SynckvseparateWrite offset: %lu,offse:%d \n", big_endian2little_endian(hashkey,KEY_SIZE),offset5>>24);
//         } 
//         offset5 = offset5 & 0xFF000FFF;
//         offset5 += ((page/4) << 12);
//         offset5 = offset5 & 0xFFFFF000;
//         value_buffer_position = 0; 
//     }

//     memcpy(value_buffer + value_buffer_position, hashvalue, VAL_SIZE);
//     value_buffer_position += VAL_SIZE;

//     offset5++;
//     block_information[offset5>>24].first++;

//     if(big_endian2little_endian(hashkey,KEY_SIZE) == 1655)
//     {
//         printf("offset5: %u\n", offset5);
//     }

//     uint32_t BlockId = offset5 >> 24;
//     uint32_t data = offset5 & 0xFFFFFF;

//     uint32_t pageID = data >> 12;  
//     uint32_t offsetInPage = data & 0xFFF;  
//     data = BlockId* pageID * (buffer_size/VAL_SIZE) + offsetInPage;
    

//     int length = EncodeLength(data);
//     for (int i = length ; i > 0; i--) 
//     {
//         char encoded = data & 0x7F; 
//         data >>= 7;  
//         if (i == 1)
//         {
//             encoded |= 0x80;
//         }
//         temp[length-i] = encoded; 
//     }
    

//     if (key_buffer_position+KEY_SIZE+length > buffer_size) 
//     {
//         uint64_t previous_block = key_block_id;
//         kv_write_queue(key_buffer,key_block_id,KEY_OCSDD_DATA_WRITE);
//         uint64_t page = BlockWritePointers[previous_block];
//         if (page == num_data_page * sectors_per_page) 
//         {
//             offset2 = (offset2 & 0x00FFFFFF) | (key_block_id << 24);
//             // printf("SynckvseparateWrite offset: %lu,offse:%d \n", big_endian2little_endian(hashkey,KEY_SIZE),offset2>>24);
//         }
//         offset2 = offset2 & 0xFF000000;
//         key_buffer_position = 0;
//     }

//     block_information[key_block_id].first++;
//     memcpy(key_buffer + key_buffer_position, hashkey, KEY_SIZE);
//     key_buffer_position += KEY_SIZE;

//     memcpy(key_buffer + key_buffer_position,temp, length);
//     key_buffer_position += length;

//     offset2++;

//     block = offset2;
//     writes_ram += (KEY_SIZE+VAL_SIZE+length);

//     return offset5;
}

uint32_t SyncvseparateWrite(const char* hashvalue, uint32_t block)
{
//     int i = 3;
//     char temp;
//     uint32_t off =0;
//     writes_ram++;

//     if (value_buffer_position+VAL_SIZE > buffer_size) 
//     {
//         uint64_t previous_block = value_block_id;
//         kv_write_queue(value_buffer,value_block_id,VALUE_OCSDD_DATA_WRITE);
//         uint64_t pages_point = BlockWritePointers[previous_block] + previous_block*geometry.clba;
//         uint64_t page = BlockWritePointers[previous_block];
//         if (pages_point == num_data_page * sectors_per_page) 
//         {
//             offset5 = (offset5 & 0x00FFFFFF) | (value_block_id << 24);
//         }
//         offset5 = offset5 & 0xFF000FFF;
//         offset5 += ((page/4) << 12);
//         offset5 = offset5 & 0xFFFFF000;
//         value_buffer_position = 0;
//     }

//     memcpy(value_buffer + value_buffer_position, hashvalue, VAL_SIZE);
//     value_buffer_position += VAL_SIZE;

//     offset5++;
//     block_information[offset5>>24].first++;
    
//     uint64_t BlockId = block>>24;
//     block_information[BlockId].second++;
    
//     if (BufferLog[BlockId].size()+7 > buffer_size)
//     {
//         // printf("BufferLog[BlockId].size(): %lu offset:%d\n", BufferLog[BlockId].size(),block);
//         memcpy(log_data_buffer, BufferLog[BlockId].data(), BufferLog[BlockId].size());
//         // printf("BlockId:%lu\n",BlockId);
//         kv_log_queue(log_data_buffer, BlockId);
//         BufferLog[BlockId].clear();
//     }

//     block = block >> 8;
//     while (i--)
//     {
//         temp = (char)(block & 0XFF);
//         block = block >> 8;
//         BufferLog[BlockId].emplace_back(temp);
//     }

//     i=4;
//     off = offset5;
//     while (i--)
//     {
//         temp = (char)(off & 0XFF);
//         off = off >> 8;
//         BufferLog[BlockId].emplace_back(temp);
//     }

//     return offset5;
}


uint32_t SyncvvariableseparateWrite(const char* hashvalue, uint32_t block)
{

//     if (value_buffer_position+VAL_SIZE > buffer_size) 
//     {
//         uint64_t previous_block = value_block_id;
//         kv_write_queue(value_buffer,value_block_id,VALUE_OCSDD_DATA_WRITE);
//         uint64_t pages_point = BlockWritePointers[previous_block] + previous_block*geometry.clba;
//         uint64_t page = BlockWritePointers[previous_block];
//         WBufferId = pages_point;
//         if (pages_point == num_data_page * sectors_per_page) 
//         {
//             offset5 = (offset5 & 0x00FFFFFF) | (value_block_id << 24);
//         }
//         offset5 = offset5 & 0xFF000FFF;
//         offset5 += ((page/4)<<12);
//         offset5 = offset5 & 0xFFFFF000;
//         value_buffer_position = 0;
//     }

//     memcpy(value_buffer + value_buffer_position, hashvalue, VAL_SIZE);
//     value_buffer_position += VAL_SIZE;

//     offset5++;
//     block_information[offset5>>24].first++;

    
//     uint64_t BlockId = offset5 >> 24;
//     uint32_t data = offset5 & 0x00FFFFFF;
//     uint32_t pageID = data >> 12;  
//     uint32_t offsetInPage = data & 0xFFF;  
//     data = BlockId* pageID * (buffer_size/VAL_SIZE) + offsetInPage;
//     block_information[BlockId].first++;
//     int length = EncodeLength(data);

//     uint64_t block_id = block>>24;
//     block = block & 0x00FFFFFF;
//     int length2 = EncodeLength(block);
//     writes_ram += (VAL_SIZE+length+length2);
//     int len = length+length2;
//     if (BufferLog[block_id].size()+ len > buffer_size)
//     {
//         memcpy(log_data_buffer, BufferLog[block_id].data(), BufferLog[block_id].size());
//         kv_log_queue(log_data_buffer, block_id);
//         BufferLog[block_id].clear();
//     }

//     for (int i = length2; i > 0; i--) 
//     {
//         char encoded = block & 0x7F; 
//         block >>= 7;  
//         if (i == 0) 
//         {
//             encoded |= 0x80;
//         }
//         BufferLog[block_id].emplace_back(encoded);
//     }

//     for (int i = length; i > 0; i--) 
//     {
//         char encoded = data & 0x7F; 
//         data >>= 7;  
//         if (i == 0) 
//         {
//             encoded |= 0x80;
//         }
//         BufferLog[block_id].emplace_back(encoded);
//     }

//     return offset5;
}

void write_file(const std::string& file_path, const uint32_t offset) 
{
//     std::ofstream outfile1(file_path, std::ios_base::app);

//     if (!outfile1) 
//     {
//         std::cerr << "Can not open the file " << file_path << "\n";
//         return;
//     }
//     outfile1 << offset << "\n";
}


/**
 *  ================= Synchronous delete module====================  
 **/
int  SynckvvariableDelete(uint32_t offset)
{ 
//     if(test11)
//         write_file("/home/TiOCS/src/data/delete_test.txt", offset);
//     uint64_t BlockId = offset >> 24;
//     uint32_t data = offset & 0x00FFFFFF;
//     uint32_t pageID = data >> 12;  
//     uint32_t offsetInPage = data & 0xFFF;  
//     data = pageID * (buffer_size/VAL_SIZE) + offsetInPage;  
//     block_information[BlockId].second++;
        
//     int length = EncodeLength(data);
//     if(length>3)
//     {
//         printf("length:%d\n",length);
//         printf("offset:%u\n",offset);
//         exit(-1);
//     }
//     char encoded;
//     resets += length;
    
//     if(BufferLog[BlockId].size() + length > buffer_size) 
//     {
//         // printf("BufferLog[BlockId].size(): %lu offset:%d\n", BufferLog[BlockId].size(),offset>>24);
//         reads_io++;
//         memcpy(log_data_buffer, BufferLog[BlockId].data(), BufferLog[BlockId].size());
//         kv_log_queue(log_data_buffer, BlockId);
//         BufferLog[BlockId].clear();
//     }

//     for (int i = length - 1; i >= 0; i--) 
//     {
//         encoded = data & 0x7F; 
//         data >>= 7;  
//         if (i == 0) 
//         {
//             encoded |= 0x80;
//         }
//         BufferLog[BlockId].emplace_back(encoded);
//     }

//     return 0;
}

int  SynckvDelete(uint32_t offset)
{ 
//      if(test11)
//         write_file("/home/TiOCS/src/data/delete_test1.txt", offset);
//     resets++;
//     char temp;
//     int i = 3;
//     uint64_t BlockId = offset >> 24;
//     block_information[BlockId].second++;

//     if (BufferLog[BlockId].size()+i > buffer_size)
//     { 
//         // printf("BufferLog[BlockId].size(): %lu offset:%d\n", BufferLog[BlockId].size(),offset>>24);
//         reads_io++;
//         memcpy(log_data_buffer, BufferLog[BlockId].data(), BufferLog[BlockId].size());
//         kv_log_queue(log_data_buffer, BlockId);
//         BufferLog[BlockId].clear();
//     }
    
//     // offset = offset >> 8;
//     while (i--)
//     {
//         temp = (char)(offset & 0XFF);
//         offset = offset >> 8;
//         BufferLog[BlockId].emplace_back(temp);
//     }
    
//     return 0;
}

int SyncvariableDelete(uint32_t offset)
{ 
    
//     uint64_t BlockId = offset >> 24;
//     uint32_t data = offset & 0xFFFFFF;

//     uint32_t pageID = data >> 12;  
//     uint32_t offsetInPage = data & 0xFFF;  
//     data = pageID * (buffer_size/VAL_SIZE) + offsetInPage;
//     int length = EncodeLength(data);  
//     block_information[BlockId].second++;
    
//     resets += length;

//     if(BufferLog[BlockId].size() + length > buffer_size) 
//     {
//         char* LogDataBuffer = (char *)spdk_dma_malloc(sectors_per_page * geometry.clba, 0x1000, NULL);
//         if (LogDataBuffer == NULL)
//         {
//             printf("Failed to allocate memory for LogDataBuffer!\n");
//             exit(-1);
//         }
//         memcpy(LogDataBuffer, BufferLog[BlockId].data(), BufferLog[BlockId].size());
//         write_queue(LogDataBuffer, BlockId);
//         BufferLog[BlockId].clear();
//         spdk_dma_free(LogDataBuffer);
//     }
        
//     for (int i = length - 1; i >= 0; i--) 
//     {
//         char encoded = data & 0x7F; 
//         data >>= 7;  
//         if (i == 0) 
//         {
//             encoded |= 0x80;
//         }
//         BufferLog[BlockId].emplace_back(encoded);
//     }
//     return 0;

}



int SyncDelete(uint32_t offset)
{ 
//     resets++;
//     char temp;
    
//     uint64_t BlockId = offset >> 24;
//     block_information[BlockId].second++;
    
//     int i = 4;
//     while (i--)
//     {
//         temp = (char)(offset & 0XFF);
//         offset = offset >> 8;
//         BufferLog[BlockId].emplace_back(temp);
//     }

//     if (BufferLog[BlockId].size() >= (sectors_per_page * geometry.clba ))
//     {
//         char* LogDataBuffer = (char *)spdk_dma_malloc(sectors_per_page * geometry.clba, 0x1000, NULL);
//         if (LogDataBuffer == NULL)
//         {
//             printf("Failed to allocate memory for LogDataBuffer!\n");
//             exit(-1);
//         }
//         memcpy(LogDataBuffer, BufferLog[BlockId].data(), BufferLog[BlockId].size());
//         write_queue(LogDataBuffer, BlockId);
//         BufferLog[BlockId].clear();
//         spdk_dma_free(LogDataBuffer);
//     }
    
//     return 0;
}



// /**
//  *  ================= Synchronous read module====================  
//  **/
char* Read4Buffer(size_t pos)
{
//     return Pagedata[pos];
}

char*  SyncRead(uint32_t offset)
{
//     uint64_t offsetpage = ((offset>>12)&0xFFF) ;
//     uint64_t PageId = (offset>>24)*geometry.clba+ offsetpage;
//     size_t Position = (offset & 0x00000FFF)-1;

//     // printf("SyncRead offset: %u (Block: %u, Page: %u, Position: %u), PageId: %lu, Position: %zu\n", offset, (offset >> 24), ((offset >> 12) & 0xFFF), (offset & 0x00000FFF), PageId,Position);

//     if(PageId == WBufferId)
//     {
//         return Read4Buffer(Position);
//     }

//     bool IsFlag = lrucache.IsLRUPage(PageId);
//     if(true)
//     {
//         //printf("Not Found!,Cache size:%lu\n",lrucache.cache.size());
//         buffernumber++;
//         // ReadNode temp;
//         key_value_entry* ReadData = read_queue(PageId);
//         // temp.data = ReadData;
//         // temp.PageId = PageId;
//         // lrucache.put(PageId, temp);
//         key_value_entry tem1;
//         memcpy(tem1.key,ReadData[Position].key,KEY_SIZE);
//         memcpy(tem1.val,ReadData[Position].val,VAL_SIZE);
//         spdk_dma_free(ReadData);
// #ifdef TIOCS_READ_DEBUG
//         printf("ReadData[Position].key: %lu ", big_endian2little_endian(ReadData[Position].key, KEY_SIZE));
//         printf("ReadData[Position].val: %lu\n",big_endian2little_endian(ReadData[Position].val, KEY_SIZE));
// #endif
//         return tem1;    
//     }
//     else
//     {
//         // printf("Founded!Cache size:%lu\n",lrucache.cache.size());
//         key_value_entry *values = lrucache.get(PageId);
//         key_value_entry tem;
//         memcpy(tem.key,values[Position].key,KEY_SIZE);
//         memcpy(tem.val,values[Position].val,VAL_SIZE);
//         return tem;
//     }
// #ifdef LRU
//     bool IsFlag = fifocache.IsFIFOPage(PageId);
//     if(!IsFlag)
//     {
//         buffernumber++;
//         FReadNode temp;
//         key_value_entry* ReadData = key_value_entryRead(PageId);
//         temp.data = ReadData;
//         temp.PageId = PageId;
//         fifocache.put(PageId, temp);
//         return ReadData[Position];    
//     }
//     else
//     {
//         key_value_entry *values = fifocache.get(PageId);
//         return values[Position];
//     }
// #endif

}

void clearBufferLog() 
{
    // for (auto& item : BufferLog) 
    // {
    //     item.second.clear();
    // }
}

void countBufferLog()
{
    // int a = 0;
    // for (auto& item : BufferLog) 
    // {
    //     a += item.second.size();
    // }
    // printf("BufferLog size: %d\n", a);
}
