#include "asyncstore.h"
#include <unistd.h>
#include "disklayer/write_buffer.h"
#include "include/buffer.h"
#include "disklayer/io_manager/io_thread.h"
#include "disklayer/io_manager.h"
#include "include/spdk_env_init.h"
#include "disklayer/read_buffer.h"
#include "utility/types.h"


const int block_bits_count = 24;

/**
 *  ================= Asynchronous write module====================  
 **/
char* allocate_task_buffer()
{
    char* task_buffer = (char*)spdk_dma_malloc((KEY_SIZE+value_size) * my_controller.buffer_capacity, 0x1000, NULL);
    if (!task_buffer) {
        logger.log(nexio_logger::info, "\033[0;31m[ERROR]\033[0m Failed to allocate memory for task_buffer.\n");
        exit(1);  // Note: consider returning NULL and handling error in the caller
    }
    memcpy(task_buffer, combined_kv_buffer, sizeof((KEY_SIZE+value_size)) * my_controller.buffer_capacity);
    return task_buffer;
}


uint64_t async_kv_write(const char* hashkey, const char* hashvalue)
{

    // Lock buffer access to ensure thread-safety
    pthread_mutex_lock(&write_buffer_mutex);

    // Check if buffer is full and signal accordingly
    if (current_buffer_position >= my_controller.write_buffer_size) 
    {
        logger.log(nexio_logger::info, "\033[0;33m[INFO]\033[0m Write buffer is full. Signaling for I/O operations.\n");
        char* task_buffer = allocate_task_buffer();
        write_task_paramaters* task = create_io_task(task_buffer);
        uint64_t lba_num_within_block = my_controller.current_write_lba_num % my_controller.nexio_lba_uint;        
        offset5 += lba_num_within_block == 0 ? 0x1000000 : 0x00000000;
        offset5 = offset5 & 0xFFFFFFFFFF000FFF;
        offset5 += (lba_num_within_block << 12);
        offset5 = offset5 & 0xFFFFFFFFFFFF000;
        add_io_task(task);
        current_buffer_position = 0;
    }

    // Copy key-value pair to buffer
    memcpy(combined_kv_buffer+current_buffer_position, hashkey, KEY_SIZE);
    memcpy(combined_kv_buffer+current_buffer_position, hashvalue, value_size);
    current_buffer_position += (KEY_SIZE+value_size);
    offset5++;

    pthread_mutex_unlock(&write_buffer_mutex);

    return offset5;
}





char* allocate_and_copy_to_task_buffer(const char* source_buffer)
{
    if (!source_buffer) {
        logger.log(nexio_logger::error, "Invalid source buffer or size.");
        return nullptr;
    }

    char* task_buffer = (char*) spdk_dma_malloc(my_controller.write_buffer_size, 0x1000, NULL);
    if (task_buffer == NULL) {
        logger.log(nexio_logger::error, "Failed to allocate memory for task_buffer.");
        return nullptr;
    }

    memcpy(task_buffer, source_buffer, my_controller.write_buffer_size);

    return task_buffer;
}



uint64_t async_kv_separate_write(const char* hashkey, const char* hashvalue, uint64_t& key_offset_within_disk)
{

    // Check if buffer is full and signal accordingly
    pthread_mutex_lock(&write_buffer_mutex);

    if(value_position_in_buffer+value_size >= my_controller.write_buffer_size) 
    {
        logger.log(nexio_logger::info, "Value buffer is full. Signaling for I/O operations.");
        
        void* task_buffer = allocate_and_copy_to_task_buffer(value_separated_buffer); // Quickly copy the current write buffer's content to a new buffer
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer); // Create and initialize the write task parameters for separation write
        if(block_bitmaps[value_block_id].count()+ my_controller.nexio_write_uint >= num_data_page)
        {
            value_block_id = block_id_allocator++;
            block_type_tracker[value_block_id] = VALUE_BLOCK;
        }

        task->block_id = value_block_id;
        task->mode = NVME_SSD_DATA_VALUE_WRITE;
        task->taskType = IOTaskType::SEPARATION_VAL_WRITE_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal
        
        offset5 = (offset5 & 0x0000000000FFFFFF) | (value_block_id << 24);  
        offset5 = offset5 & 0xFFFFFFFFFF000FFF;
        offset5 += ((my_controller.current_write_lba_num % my_controller.nexio_lba_uint) << 12);
        offset5 = offset5 & 0xFFFFFFFFFFFFF000;

        value_position_in_buffer = 0;

    }
    memcpy(value_separated_buffer + value_position_in_buffer, hashvalue, value_size);
    value_position_in_buffer += value_size;
    offset5++;
    block_information[offset5>>24].first++;


    if (key_position_in_buffer + KEY_SIZE + sizeof(uint32_t) > my_controller.write_buffer_size) 
    {
        logger.log(nexio_logger::info, "Key buffer is full. Signaling for I/O operations.");
        void* task_key_buffer =  allocate_and_copy_to_task_buffer(key_separated_buffer);  
        separation_write_task_paramaters* key_task = create_separation_io_task(task_key_buffer);// Create and initialize the write task parameters for separation write
        if(block_bitmaps[key_block_id].count()+ my_controller.nexio_write_uint >= num_data_page)
        {
            key_block_id = block_id_allocator++;
            block_type_tracker[key_block_id] = KEY_BLOCK;
        } 
        key_task->block_id = key_block_id;
        key_task->mode = NVME_SSD_DATA_KEY_WRITE;
        key_task->taskType = IOTaskType::SEPARATION_KEY_WRITE_TASK;
        add_separation_io_task(key_task); // Add the I/O task and signal

        offset2 = (offset2 & 0x0000000000FFFFFF) | (key_block_id << 24);
        offset2 = offset2 & 0xFFFFFFFFFF000FFF;
        offset2 += ((my_controller.current_write_lba_num % my_controller.nexio_lba_uint) << 12);
        offset2 = offset2 & 0XFFFFFFFFFFFFF000;
        key_position_in_buffer = 0;
    }

    memcpy(key_separated_buffer + key_position_in_buffer, hashkey, KEY_SIZE);
    key_position_in_buffer += KEY_SIZE;
    memcpy(key_separated_buffer + key_position_in_buffer, &offset5, sizeof(uint32_t));
    key_position_in_buffer += sizeof(uint32_t);
    block_information[key_offset_within_disk >> 24].first++;
    offset2++;
    pthread_mutex_unlock(&write_buffer_mutex);

#ifdef IO_DEBUG
    // if(big_endian2little_endian(hashkey,KEY_SIZE) == 8456562)
    // {
    //     printf("offset5: %u,offset2:%u\n", offset5,offset2);
    // }
#endif

    return offset5;
}


uint64_t async_kv_separate_variable_write(const char* hashkey, const char* hashvalue, uint64_t& block)
{
    pthread_mutex_lock(&write_buffer_mutex);
    if (value_position_in_buffer+value_size > my_controller.write_buffer_size) 
    {
        void* task_buffer = allocate_and_copy_to_task_buffer(value_separated_buffer); // Quickly copy the current write buffer's content to a new buffer
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer); // Create and initialize the write task parameters for separation write
        
        if(block_bitmaps[value_block_id].count()+ my_controller.nexio_write_uint >= num_data_page)
        {
            value_block_id = block_id_allocator++;
            block_type_tracker[value_block_id] = VALUE_BLOCK;
        }

        task->block_id = value_block_id;
        task->mode = NVME_SSD_DATA_VALUE_WRITE;
        task->taskType = IOTaskType::SEPARATION_VAL_WRITE_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal

        // printf("key:%lu previous_block: %lu, pages_point: %lu, page: %lu\n",big_endian2little_endian(hashkey,KEY_SIZE),previous_block, pages_point, page);
        // WBufferId = pages_point;

        offset5 = (offset5 & 0x0000000000FFFFFF) | (value_block_id << 24); 
        offset5 = offset5 & 0xFFFFFFFFFF000FFF;
        offset5 += ((my_controller.current_write_lba_num%my_controller.nexio_lba_uint) << 12);
        offset5 = offset5 & 0xFFFFFFFFFFFFF000;
        value_position_in_buffer = 0; 
    }

    memcpy(value_separated_buffer + value_position_in_buffer, hashvalue, value_size);
    value_position_in_buffer += value_size;

    offset5++;
    block_information[offset5>>24].first++;

    // if(big_endian2little_endian(hashkey,KEY_SIZE) == 1655)
    // {
    //     printf("offset5: %u\n", offset5);
    // }

    uint64_t BlockId = offset5 >> 24;
    char temp[8];
    uint64_t page_id = offset5 >> 12;  
    uint64_t offsetInPage = offset5 & 0x0000000000000FFF;  
    uint64_t data = BlockId* page_id * (my_controller.write_buffer_size/value_size) + offsetInPage;
    

    int length = EncodeLength(data);
    assert(length <= 8);
    for (int i = length ; i > 0; i--) 
    {
        char encoded = data & 0x7F; 
        data >>= 7;  
        if (i == 1)
        {
            encoded |= 0x80;
        }
        temp[length-i] = encoded; 
    }
    

    if (key_position_in_buffer+KEY_SIZE+length > my_controller.write_buffer_size) 
    {
        void* task_buffer = allocate_and_copy_to_task_buffer(key_separated_buffer); // Quickly copy the current write buffer's content to a new buffer
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer); // Create and initialize the write task parameters for separation write
        
        if(block_bitmaps[key_block_id].count()+ my_controller.nexio_write_uint >= num_data_page)
        {
            key_block_id = block_id_allocator++;
            block_type_tracker[key_block_id] = KEY_BLOCK;
        } 

        task->block_id = key_block_id;
        task->mode = NVME_SSD_DATA_KEY_WRITE;
        task->taskType = IOTaskType::SEPARATION_KEY_WRITE_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal

        offset2 = (offset2 & 0x0000000000FFFFFF) | (key_block_id << 24); 
        offset2 = offset2 & 0xFFFFFFFFFF000FFF;
        offset2 += ((my_controller.current_write_lba_num%my_controller.nexio_lba_uint) << 12);
        offset2 = offset2 & 0xFFFFFFFFFFFFF000;
        key_position_in_buffer = 0;
    }

    block_information[key_block_id].first++;
    memcpy(key_separated_buffer + key_position_in_buffer, hashkey, KEY_SIZE);
    key_position_in_buffer += KEY_SIZE;

    memcpy(key_separated_buffer + key_position_in_buffer,temp, length);
    key_position_in_buffer += length;

    offset2++;

    block = offset2;
    pthread_mutex_unlock(&write_buffer_mutex);

    return offset5;
}



char* allocate_and_copy_to_buffer(const std::vector<char>& source_buffer) {
    if (source_buffer.empty()) {
        logger.log(nexio_logger::error, "Source buffer is empty.");
        return nullptr;
    }

    char* task_buffer = (char*) spdk_dma_malloc(my_controller.write_buffer_size, 0x1000, NULL);
    if (task_buffer == NULL) {
        logger.log(nexio_logger::error, "Failed to allocate memory for task_buffer.");
        return nullptr;
    }

    size_t copy_size = std::min(source_buffer.size(), my_controller.write_buffer_size);
    memcpy(task_buffer, source_buffer.data(), copy_size);

    return task_buffer;
}


uint64_t async_kv_separate_update(const char* new_hashvalue, uint64_t key_offset)
{
    int i = 3;
    char temp;
    uint32_t off =0;
    pthread_mutex_lock(&write_buffer_mutex);
    if(value_position_in_buffer+value_size >= my_controller.write_buffer_size) 
    {
        logger.log(nexio_logger::info, "Value buffer now is full. Signaling for I/O operations.");

        void* task_buffer = allocate_and_copy_to_task_buffer(value_separated_buffer);
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer); // Create and initialize the write task parameters for separation write
        task->taskType = IOTaskType::SEPARATION_VAL_WRITE_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal
    
        offset5 = (offset5 & 0x0000000000FFFFFF) | (value_block_id << 24); 
        offset5 = offset5 & 0xFFFFFFFFFF000FFF;
        offset5 += ((my_controller.current_write_lba_num%my_controller.nexio_lba_uint) << 12);
        offset5 = offset5 & 0xFFFFFFFFFFFFF000;
        value_position_in_buffer = 0;
    }

    memcpy(value_separated_buffer + value_position_in_buffer, new_hashvalue, value_size);
    value_position_in_buffer += value_size;
    offset5++;
    block_information[offset5>>24].first++;
    
    uint64_t block_id = key_offset>>24;
    block_information[block_id].second++;
    
    if (log_buffer[block_id].size()+10 > my_controller.write_buffer_size) 
    {
        // printf("log_buffer[BlockId].size(): %lu offset:%d\n", log_buffer[BlockId].size(),block);
        
        void* task_buffer =  allocate_and_copy_to_buffer(log_buffer[block_id]);// Quickly copy the current write buffer's content to a new buffer
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer); // Create and initialize the write task parameters for separation write
        add_separation_io_task(task);  // Add the I/O task and signal
    }

    key_offset = key_offset >> 8;
    while (i--)
    {
        temp = (char)(key_offset & 0XFF);
        key_offset = key_offset >> 8;
        log_buffer[block_id].emplace_back(temp);
    }

    i=4;
    off = offset5;
    while (i--)
    {
        temp = (char)(off & 0XFF);
        off = off >> 8;
        log_buffer[block_id].emplace_back(temp);
    }
    pthread_mutex_unlock(&write_buffer_mutex);

    return offset5;
}



uint64_t async_kv_separate_variable_update(const char* new_hashvalue, uint64_t& block)
{
    pthread_mutex_lock(&write_buffer_mutex);
    if (value_position_in_buffer+value_size > my_controller.write_buffer_size) 
    {
        logger.log(nexio_logger::info, "Value buffer now is full. Signaling for I/O operations.");
        void* task_buffer = allocate_and_copy_to_task_buffer(value_separated_buffer);
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer); // Create and initialize the write task parameters for separation write
        task->taskType = IOTaskType::SEPARATION_VAL_WRITE_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal
        value_position_in_buffer = 0;

        offset5 = (offset5 & 0x00FFFFFF) | (value_block_id << 24);
        offset5 = offset5 & 0xFF000FFF;
        offset5 += ((my_controller.nexio_lba_uint/my_controller.nexio_write_uint) << 12);
        offset5 = offset5 & 0xFFFFF000;
    }

    memcpy(value_separated_buffer + value_position_in_buffer, new_hashvalue, value_size);
    value_position_in_buffer += value_size;

    offset5++;
    block_information[offset5>>24].first++;

    
    uint64_t BlockId = offset5 >> 24;
    uint32_t data = offset5 & 0x00FFFFFF;
    uint32_t pageID = data >> 12;  
    uint32_t offsetInPage = data & 0xFFF;  
    data = BlockId* pageID * (my_controller.write_buffer_size/value_size) + offsetInPage;
    block_information[BlockId].first++;
    int length = EncodeLength(data);

    uint64_t block_id = block>>24;
    block = block & 0x0000000000FFFFFF;
    int length2 = EncodeLength(block);
    int len = length+length2;
    if (log_buffer[block_id].size()+ len > my_controller.write_buffer_size)
    {

        char* task_buffer= allocate_and_copy_to_buffer(log_buffer[block_id]);
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer);
        task->block_id = block_id;
        task->mode = NVME_SSD_DATA_LOG_WRITE;
        task->taskType = IOTaskType::SEPARATION_LOG_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal

        log_buffer[block_id].clear();

    }

    for (int i = length2; i > 0; i--) 
    {
        char encoded = block & 0x7F; 
        block >>= 7;  
        if (i == 0) 
        {
            encoded |= 0x80;
        }
        log_buffer[block_id].emplace_back(encoded);
    }

    for (int i = length; i > 0; i--) 
    {
        char encoded = data & 0x7F; 
        data >>= 7;  
        if (i == 0) 
        {
            encoded |= 0x80;
        }
        log_buffer[block_id].emplace_back(encoded);
    }
    pthread_mutex_unlock(&write_buffer_mutex);
    return offset5;
}








/**
 *  ================= Synchronous delete module====================  
 **/
char* allocate_log_space_and_copy(uint64_t blockId, const std::unordered_map<uint64_t, std::vector<char>>& log_buffer) {
    auto it = log_buffer.find(blockId);
    if (it == log_buffer.end()) {
        printf("BlockId not found in log_buffer!\n");
        return nullptr;
    }

    const std::vector<char>& logData = it->second;
    
    char* log_data_buffer = (char *)spdk_dma_malloc(my_controller.write_buffer_size, 0x1000, NULL);
    if (log_data_buffer == NULL) {
        printf("Failed to allocate memory for LogDataBuffer!\n");
        return nullptr;
    }
    
    size_t copy_size = std::min(logData.size(), static_cast<size_t>(my_controller.write_buffer_size));
    memcpy(log_data_buffer, logData.data(), copy_size);
    
    return log_data_buffer;
}



int  async_kv_separte_variable_delete(uint64_t offset)
{ 

    uint64_t BlockId = offset >> 24;
    uint32_t data = offset & 0x00FFFFFF;
    uint32_t pageID = data >> 12;  
    uint32_t offsetInPage = data & 0xFFF;  
    data = pageID * (my_controller.write_buffer_size/value_size) + offsetInPage;  
    block_information[BlockId].second++;
        
    int length = EncodeLength(data);
    if(length>3)
    {
        printf("length:%d\n",length);
        printf("offset:%lu\n",offset);
        exit(-1);
    }
    char encoded;
    
    if(log_buffer[BlockId].size() + length > my_controller.write_buffer_size) 
    {
        // printf("log_buffer[BlockId].size(): %lu offset:%d\n", log_buffer[BlockId].size(),offset>>24);

        char* task_buffer = allocate_log_space_and_copy(BlockId,log_buffer);

        separation_write_task_paramaters* task = create_separation_io_task(task_buffer);
        task->block_id = BlockId;
        task->mode = NVME_SSD_DATA_LOG_WRITE;
        task->taskType = IOTaskType::SEPARATION_LOG_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal
        
        log_buffer[BlockId].clear();
    }

    for (int i = length - 1; i >= 0; i--) 
    {
        encoded = data & 0x7F; 
        data >>= 7;  
        if (i == 0) 
        {
            encoded |= 0x80;
        }
        log_buffer[BlockId].emplace_back(encoded);
    }

    return 0;
}

int  async_kv_separate_delete(uint32_t offset)
{ 

    char temp;
    int i = 3;
    uint64_t BlockId = offset >> 24;
    block_information[BlockId].second++;

    if (log_buffer[BlockId].size()+i > my_controller.write_buffer_size)
    { 
        // printf("log_buffer[BlockId].size(): %lu offset:%d\n", log_buffer[BlockId].size(),offset>>24);

        char* task_buffer = allocate_log_space_and_copy(BlockId,log_buffer);
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer);
        task->block_id = BlockId;
        task->mode = NVME_SSD_DATA_LOG_WRITE;
        task->taskType = IOTaskType::SEPARATION_LOG_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal
        log_buffer[BlockId].clear();
    }
    
    // offset = offset >> 8;
    while (i--)
    {
        temp = (char)(offset & 0XFF);
        offset = offset >> 8;
        log_buffer[BlockId].emplace_back(temp);
    }
    
    return 0;
}

int async_kv_variable_Delete(uint32_t offset)
{ 
    
    uint64_t BlockId = offset >> 24;
    uint32_t data = offset & 0xFFFFFF;

    uint32_t pageID = data >> 12;  
    uint32_t offsetInPage = data & 0xFFF;  
    data = pageID * (my_controller.write_buffer_size/value_size) + offsetInPage;
    int length = EncodeLength(data);  
    block_information[BlockId].second++;
    

    if(log_buffer[BlockId].size() + length > my_controller.write_buffer_size) 
    {
        char* task_buffer = allocate_log_space_and_copy(BlockId,log_buffer);
        write_task_paramaters* task = create_io_task(task_buffer);
        task->taskType = IOTaskType::SEPARATION_LOG_TASK;
        add_io_task(task);  // Add the I/O task and signal
        log_buffer[BlockId].clear();

    }
        
    for (int i = length - 1; i >= 0; i--) 
    {
        char encoded = data & 0x7F; 
        data >>= 7;  
        if (i == 0) 
        {
            encoded |= 0x80;
        }
        log_buffer[BlockId].emplace_back(encoded);
    }
    return 0;

}




uint64_t async_kv_delete(uint64_t offset)
{ 
    // Calculate the BlockId based on the offset
    uint64_t BlockId = offset >> block_bits_count;
    block_information[BlockId].second++;
    
    // Convert offset to char and store in log_buffer
    char temp;
    int i = 4;
    while (i--)
    {
        temp = (char)(offset & 0XFF);
        offset = offset >> 8;
        log_buffer[BlockId].emplace_back(temp);
    }

    pthread_mutex_lock(&write_buffer_mutex);
    // Check if the log buffer for the block is full
    if (log_buffer[BlockId].size() >= my_controller.write_buffer_size)
    {
        char* task_buffer = allocate_log_space_and_copy(BlockId,log_buffer);
        separation_write_task_paramaters* task = create_separation_io_task(task_buffer);
        task->block_id = BlockId;
        task->mode = NVME_SSD_DATA_LOG_WRITE;
        task->taskType = IOTaskType::SEPARATION_LOG_TASK;
        add_separation_io_task(task);  // Add the I/O task and signal
        log_buffer[BlockId].clear();
    }

    pthread_mutex_unlock(&write_buffer_mutex);
    
    return 0; 
}





char*  async_read(uint64_t offset)
{
    uint64_t offsetpage = ((offset>>12)&0xFFF) ;
    uint64_t PageId = (offset>>block_bits_count)*my_controller.nexio_lba_uint+ offsetpage;
    size_t Position = (offset & 0x00000FFF)-1;

    // printf("SyncRead offset: %u (Block: %u, Page: %u, Position: %u), PageId: %lu, Position: %zu\n", offset, (offset >> 24), ((offset >> 12) & 0xFFF), (offset & 0x00000FFF), PageId,Position);

    // if(PageId == WBufferId)
    // {
    //     return Read4Buffer(Position);
    // }

    bool IsFlag = lrucache.IsLRUPage(PageId);
    if(true)
    {
        //printf("Not Found!,Cache size:%lu\n",lrucache.cache.size());
        // buffernumber++;
        // ReadNode temp;
        // key_value_entry* ReadData = read_queue(PageId);
        // temp.data = ReadData;
        // temp.PageId = PageId;
        // lrucache.put(PageId, temp);
        // key_value_entry tem1;
        // memcpy(tem1.key,ReadData[Position].key,KEY_SIZE);
        // memcpy(tem1.val,ReadData[Position].val,value_size);
        // spdk_dma_free(ReadData);
#ifdef TIOCS_READ_DEBUG
        printf("ReadData[Position].key: %lu ", big_endian2little_endian(ReadData[Position].key, KEY_SIZE));
        printf("ReadData[Position].val: %lu\n",big_endian2little_endian(ReadData[Position].val, KEY_SIZE));
#endif
        return nullptr;    
    }
    else
    {
        // printf("Founded!Cache size:%lu\n",lrucache.cache.size());
        // key_value_entry *values = lrucache.get(PageId);
        // key_value_entry tem;
        // memcpy(tem.key,values[Position].key,KEY_SIZE);
        // memcpy(tem.val,values[Position].val,VAL_SIZE);
        return nullptr;
    }
#ifdef LRU
    bool IsFlag = fifocache.IsFIFOPage(PageId);
    if(!IsFlag)
    {
        buffernumber++;
        FReadNode temp;
        key_value_entry* ReadData = key_value_entryRead(PageId);
        temp.data = ReadData;
        temp.PageId = PageId;
        fifocache.put(PageId, temp);
        return ReadData[Position];    
    }
    else
    {
        key_value_entry *values = fifocache.get(PageId);
        return values[Position];
    }
#endif

}