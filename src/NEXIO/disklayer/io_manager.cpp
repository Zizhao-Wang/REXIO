#include "io_manager.h"
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "memlayer/syncstore.h"
#include "write_buffer.h"
#include "include/buffer.h"
#include "include/spdk_env_init.h"
#include "include/log.h"


int out_stand = 0;

size_t key_num_data_page = 0;

size_t value_num_data_page = 0;

struct spdk_nvme_qpair *qpair = nullptr;

std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkLog;

std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkData;

std::unordered_map<uint64_t, uint64_t> block_current_write_pointers;

std::vector<std::bitset<SPDK_LBAs_IN_NEXIO_LBA>> block_bitmaps;

std::unordered_map<uint64_t, std::pair<int,int>> block_information;

const int IO_QUEUE_THRESHOLD = 10;  

/**  
 * ================= initialization   module ====================  
 **/
void init_io_manager()
{
	uint64_t nexio_lba_num = device_info->ns_info_array[0].ns_lba_count /my_controller.nexio_lba_uint;
	for (uint64_t i = 0; i < nexio_lba_num; ++i) {
        block_current_write_pointers[i] = 0;
    }

	size_t total_blocks = device_info->ns_info_array[0].ns_lba_count / my_controller.nexio_lba_uint;
	block_bitmaps.resize(total_blocks);
    for (uint64_t i = 0; i < total_blocks; ++i) {
        block_bitmaps[i].reset();
    }

	logger.log(nexio_logger::info, "Successfully initialized I/O manager.");
}


int pointer_Renew(int mode, uint64_t log_pointer = 0)
{

	if(mode == NVME_SSD_CONTROLLER && log_pointer == 0)
	{
		// BlockWritePointers[page_pointer/geometry.clba]= BlockWritePointers[page_pointer/geometry.clba] + sectors_per_page;
        // if(ChunkData[page_pointer/geometry.clba].size()>=num_data_page)
        // {
        //     page_pointer += (geometry.clba-(page_pointer%geometry.clba));
        // }
		// else
		// {
		// 	page_pointer += sectors_per_page;
		// }
	}
	else if(mode == NVME_SSD_LOG_WRITE)
	{
		// page_pointer += (log_pointer/geometry.clba == page_pointer/geometry.clba)?sectors_per_page:0;
        // BlockWritePointers[log_pointer/geometry.clba]= BlockWritePointers[log_pointer/geometry.clba] + sectors_per_page;
	}
   
    return 0;
}

int pointer_Renew(int mode, uint64_t current_write_pointer,uint64_t& block_id, uint64_t log_pointer = 0)
{
	
	if(mode == NVME_SSD_CONTROLLER)
	{
		// BlockWritePointers[block_id]= BlockWritePointers[block_id] + sectors_per_page;
        // if(ChunkData[block_id].size()>=num_data_page)
        // {
        //     block_id = UINT64_MAX;
        // }
	}
	else 
	{
		// if(BlockWritePointers[block_id] >= geometry.clba)
		// {
		// 	printf("execeed the block size!\n");
		// 	exit(-1);
		// }
        // BlockWritePointers[block_id]= BlockWritePointers[block_id] + sectors_per_page;
		
	}
    return 0;
}


void lba_update(int mode, uint64_t log_lba_pointers = 0)
{

	if(mode == NVME_SSD_DATA_WRITE && log_lba_pointers == 0)
	{
		uint64_t nexio_lba_num = my_controller.current_write_lba_num/my_controller.nexio_lba_uint;
		block_current_write_pointers[nexio_lba_num] += my_controller.nexio_write_uint;
		my_controller.current_write_lba_num += my_controller.nexio_write_uint;
        if(block_bitmaps[nexio_lba_num].count()>=500)
        {
            logger.log(nexio_logger::debug, "my_controller.current_write_lba_num before addition: " + std::to_string(my_controller.current_write_lba_num));
			logger.log(nexio_logger::debug, "my_controller.nexio_lba_uint: " + std::to_string(my_controller.nexio_lba_uint));
			logger.log(nexio_logger::debug, "block_current_write_pointers[nexio_lba_num]: " + std::to_string(block_current_write_pointers[nexio_lba_num]));
			my_controller.current_write_lba_num += (my_controller.nexio_lba_uint - block_current_write_pointers[nexio_lba_num]);
			logger.log(nexio_logger::debug, "my_controller.current_write_lba_num after addition: " + std::to_string(my_controller.current_write_lba_num));
		}
	}
	else if(mode == NVME_SSD_LOG_WRITE)
	{
		uint64_t log_lba_num  = log_lba_pointers/my_controller.nexio_lba_uint;
		uint64_t data_lba_num = my_controller.current_write_lba_num/my_controller.nexio_lba_uint;
		my_controller.current_write_lba_num += (log_lba_num == data_lba_num) ? my_controller.nexio_write_uint:0;
		block_current_write_pointers[log_lba_num] += my_controller.nexio_write_uint;
	}
}

void lba_update(int mode, uint64_t current_write_pointer, uint64_t block_id, uint64_t log_pointer = 0)
{

	block_current_write_pointers[block_id] += my_controller.nexio_write_uint;
	if(mode == NVME_SSD_DATA_LOG_WRITE)
	{
		
	}
	else 
	{
		if(block_current_write_pointers[block_id] >= my_controller.nexio_lba_uint){
			logger.log(nexio_logger::error, "Execeed the block size!\n");
		}
        block_current_write_pointers[block_id] += my_controller.nexio_write_uint;
	}
}





/**  
 * ================= DATA WRITE(I/O)  module ====================  
 **/

std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


void IO_write_complete(void *arg, const struct spdk_nvme_cpl *completion)
{

    IO_context* context = static_cast<IO_context*>(arg);
    if (spdk_nvme_cpl_is_error(completion)) {
        logger.log(nexio_logger::error, "OCSSD vector write failed with status 0x" + std::to_string(completion->status.sct));
        delete context; 
        return;
    }

    out_stand--;
    total_write_bytes = context->io_size;
    delete context;  

    return ;

}

int write_queue()
{

    uint64_t lba_start = 0;
    uint64_t lba_count = 0;
    char *data =nullptr;
    if (spdk_nvme_ns_cmd_write(ns, qpair, data, lba_start, lba_count, IO_write_complete, NULL, 0) == 0){
        out_stand++;
        logger.log(nexio_logger::info, "Successfully sent write request to OCSSD.");
        
    }
    else{
        logger.log(nexio_logger::error, "Failed to write data to OCSSD!");
        return -1;
    }

    while (out_stand)
    {
        spdk_nvme_qpair_process_completions(qpair, 0);
    }
    logger.log(nexio_logger::info, "IO operation completed.");

    // size_t start_index = my_controller.current_write_lba_num / my_controller.nexio_lba_uint;
    // for (size_t i = start_index; i < start_index + lba_count && i < block_bitmap.size(); ++i) 
    // {
    //     block_bitmap[i] = 1;
    // }
    // ChunkData[page_pointer/geometry.clba].emplace_back(page_pointer);
    pointer_Renew(NVME_SSD_DATA_WRITE);

    return 0;
}


int write_queue(uint64_t lba_start, uint64_t lba_count, void* write_buffer)
{
	if (out_stand >= IO_QUEUE_THRESHOLD) 
	{
		while(out_stand>0){
			spdk_nvme_qpair_process_completions(qpair, 0);
		}
		assert(out_stand == 0);
    }

    IO_context* context = new IO_context;
    context->io_size = lba_count*device_info->ns_info_array[0].lba_size;
	
    int flag = spdk_nvme_ns_cmd_write(device_info->ns_info_array[0].ns, qpair, write_buffer, lba_start, lba_count, IO_write_complete, context, 0);
    if (flag == 0){
        out_stand++;
        logger.log(nexio_logger::info, "Successfully sent write request to OCSSD. LBA Start: " + std::to_string(lba_start) + ", LBA Count: " + std::to_string(lba_count));
    }
    else{
        logger.log(nexio_logger::error, "Failed to write data to OCSSD at LBA Start: " + std::to_string(lba_start) + " with LBA Count: " + std::to_string(lba_count));
        delete context;
        return -1;
    }

    size_t start_index = lba_start % my_controller.nexio_lba_uint;
    size_t block_num = lba_start / my_controller.nexio_lba_uint;
    for (size_t i = start_index; i < start_index + lba_count && i < block_bitmaps[block_num].size(); ++i){
        block_bitmaps[block_num][i] = 1;
    }
    logger.log(nexio_logger::debug, "Updated block bitmap from index: " + std::to_string(start_index) + " for LBA Count: " + std::to_string(lba_count) + " bit setted: " + std::to_string(block_bitmaps[block_num].count()));
    
    lba_update(NVME_SSD_DATA_WRITE);

    return 0;
}

void kv_page_write_complete(void *arg, const struct spdk_nvme_cpl *completion)
{
    IO_context* context = static_cast<IO_context*>(arg);
    if (spdk_nvme_cpl_is_error(completion)) {
        
        logger.log(nexio_logger::error, "OCSSD vector write failed with status 0x" + std::to_string(completion->status.sct));
        delete context; 
        return;
    }

    out_stand--;
    // total_write_bytes += context->io_size;
    delete context;  

    return ;
}

void process_pending_spdk_io() {

    while (out_stand > 0) {
        spdk_nvme_qpair_process_completions(qpair, 0);
    }

    assert(out_stand == 0);
    
    printf("All pending SPDK I/O operations have been processed.\n");
}



int kv_write_queue(void* write_buffer, uint64_t block_id, int mode)
{

	if(block_id == UINT64_MAX)
    {
        logger.log(nexio_logger::error, "Error: block_id is set to an invalid value (UINT64_MAX). Current block_id: " + std::to_string(block_id));
        return -1;
    }

	uint64_t lba_start = block_current_write_pointers[block_id] + (block_id)*my_controller.nexio_lba_uint;
	uint64_t lba_count = my_controller.nexio_write_uint;

	if (out_stand >= IO_QUEUE_THRESHOLD){
		while(out_stand>0){
			spdk_nvme_qpair_process_completions(qpair, 0);
		}
		assert(out_stand == 0);
    }


    IO_context* context = new IO_context;
    context->io_size = lba_count*device_info->ns_info_array[0].lba_size;

	int flag = spdk_nvme_ns_cmd_write(device_info->ns_info_array[0].ns, qpair, write_buffer, lba_start, lba_count, kv_page_write_complete, context, 0);
	if (flag == 0){
    	out_stand++;
        if(mode == NVME_SSD_DATA_KEY_WRITE){
            std::string currentTime = getCurrentTimeString();
            logger.log(nexio_logger::info, 
                "Time: " + currentTime + " - Successfully sent write request to OCSSD. LBA Start: " + std::to_string(lba_start) + ", LBA Count: " + std::to_string(lba_count), 
                "key"); 
        }
        else if(mode == NVME_SSD_DATA_VALUE_WRITE){
            std::string currentTime = getCurrentTimeString();
            logger.log(nexio_logger::info, 
                "Time: " + currentTime + " - Successfully sent write request to OCSSD. LBA Start: " + std::to_string(lba_start) +" "+ std::to_string(lba_start/128)+ ", LBA Count: " + std::to_string(lba_count), 
                "val");
        }
    } else {
        if(mode == NVME_SSD_DATA_KEY_WRITE){
            logger.log(nexio_logger::error, "Failed to write data to OCSSD at LBA Start: " + std::to_string(lba_start) + " with LBA Count: " + std::to_string(lba_count),"key");
        }
        else if(mode == NVME_SSD_DATA_VALUE_WRITE){
            logger.log(nexio_logger::error, "Failed to write data to OCSSD at LBA Start: " + std::to_string(lba_start) + " with LBA Count: " + std::to_string(lba_count),"val");
        }
    	
    	return -1;
	}


	size_t start_index = lba_start % my_controller.nexio_lba_uint;
    size_t block_num = lba_start / my_controller.nexio_lba_uint;
    for (size_t i = start_index; i < start_index + lba_count && i < block_bitmaps[block_num].size(); ++i){
        block_bitmaps[block_num][i] = 1;
    }

    if(mode == NVME_SSD_DATA_KEY_WRITE){
        logger.log(nexio_logger::debug, "Updated block bitmap from index: " + std::to_string(start_index) + " for LBA Count: " + std::to_string(lba_count) + " bit setted: " + std::to_string(block_bitmaps[block_num].count()),"key");
    }
    else if(mode == NVME_SSD_DATA_VALUE_WRITE){
        logger.log(nexio_logger::debug, "Updated block bitmap from index: " + std::to_string(start_index) + " for LBA Count: " + std::to_string(lba_count) + " bit setted: " + std::to_string(block_bitmaps[block_num].count()),"val");
    }
    
    // update pointers within a block 
    block_current_write_pointers[block_id] += my_controller.nexio_write_uint;

	return 0;
}




/**  
 * ================= LOG WRITE(I/O) module ====================  
 **/

void log_write_complete(void *arg, const struct spdk_nvme_cpl *completion) {
    IO_context* context = static_cast<IO_context*>(arg);
    if (spdk_nvme_cpl_is_error(completion)) {
        
        logger.log(nexio_logger::error, "SSD log vector write failed with status 0x" + std::to_string(completion->status.sct));
        delete context; 
        return;
    }

    out_stand--;
    // total_write_bytes += context->io_size;
    delete context;  
}


int write_queue(char *write_buffer, uint64_t block_id)
{
	
	// uint64_t current_writer_pointer = BlockWritePointers[block_id] + block_id*geometry.clba;
	// printf("current_writer_pointer:%lu BlockWritePointers[block_id]:%lu block_id:%lu\n", current_writer_pointer,BlockWritePointers[block_id],block_id);

    uint64_t lba_start = 0;
	uint64_t lab_count = 0;


    if (spdk_nvme_ns_cmd_write(ns, qpair, write_buffer, lba_start, lab_count, log_write_complete, NULL, 0) == 0)
    {
        out_stand++;
    }
    else
    {
        printf("Failed to write data to OCSSD!\n");
		return -1;
    }

    while (out_stand)
	{
		spdk_nvme_qpair_process_completions(qpair, 0);
	}

	// pointer_Renew(OCSSD_LOG_WRITE_TIOCS, current_writer_pointer);


    return 0;
}

int kv_log_queue(char *write_buffer, uint64_t block_id)
{
	

    if (out_stand >= IO_QUEUE_THRESHOLD){
		while(out_stand>0){
			spdk_nvme_qpair_process_completions(qpair, 0);
		}
		assert(out_stand == 0);
    }
	uint64_t current_log_writer_pointer = block_current_write_pointers[block_id] + block_id*my_controller.nexio_lba_uint;
// 	printf("current_writer_pointer:%lu BlockWritePointers[block_id]:%lu block_id:%lu in kv_log_queue\n", current_log_writer_pointer,block_current_write_pointers[block_id],block_id);
// #ifdef IO_DEBUG
// 	// if(block_id)
// 	// {
// 		// printf("current_writer_pointer:%lu BlockWritePointers[block_id]:%lu block_id:%lu in kv_log_queue\n", current_writer_pointer,BlockWritePointers[block_id],block_id);
// 	// }
// #endif
    uint64_t lba_start = current_log_writer_pointer;
	uint64_t lba_count = my_controller.nexio_log_unit;

    IO_context* context = new IO_context;
    context->io_size = lba_count*device_info->ns_info_array[0].lba_size;

    if (spdk_nvme_ns_cmd_write(ns, qpair, write_buffer, lba_start, lba_count, log_write_complete, context, 0) == 0) {
    	out_stand++;
	} else {
    	logger.log(nexio_logger::error, "Failed to write data to OCSSD!");
    	return -1;
	}


    size_t start_index = lba_start % my_controller.nexio_lba_uint;
    size_t block_num = lba_start / my_controller.nexio_lba_uint;
    for (size_t i = start_index; i < start_index + lba_count && i < block_bitmaps[block_num].size(); ++i){
        block_bitmaps[block_num][i] = 1;
    }
    block_current_write_pointers[block_id] += my_controller.nexio_log_unit;

    return 0;
}
 



/**  
 * ================= READ(I/O) module ====================  
 **/

void page_read_complete(void *arg, const struct spdk_nvme_cpl *completion) {
    if (spdk_nvme_cpl_is_error(completion)) {
        logger.log(nexio_logger::error, "OCSSD page read failed with status: " + std::to_string(completion->status.sct));
    }
    out_stand--;
}

char* read_queue(uint64_t page_id) {

    #ifdef DEBUG_READ_QUEUE
    printf("Starting read queue, page_id: %lu\n", page_id);
    #endif

    char *buffer = (char *)spdk_dma_malloc(SPDK_LBAs_IN_NEXIO_WRITE_BUFFER*512, 0x1000, NULL);
    if (buffer == NULL) {
        logger.log(nexio_logger::error, "Failed to allocate buffer memory in read_queue!");
        return nullptr;
    }

    #ifdef DEBUG_READ_QUEUE
    printf("Buffer memory allocated successfully, size: %lu bytes\n", SPDK_LBAs_IN_NEXIO_WRITE_BUFFER * 512);
    #endif

    uint64_t lba_start = page_id * SPDK_LBAs_IN_NEXIO_WRITE_BUFFER;
    uint64_t lba_count = SPDK_LBAs_IN_NEXIO_WRITE_BUFFER;

    #ifdef DEBUG_READ_QUEUE
    printf("LBA start position: %lu, LBA count: %lu\n", lba_start, lba_count);
    #endif

    int read_result = spdk_nvme_ns_cmd_read(ns, qpair, (void *)buffer, lba_start, lba_count, page_read_complete, NULL, 0);
    if (read_result == 0) {
        out_stand++;
        logger.log(nexio_logger::info, "Successfully sent read request to SSD.");
    } else {
        logger.log(nexio_logger::error, "Failed to read data from SSD!");
        spdk_dma_free(buffer);

        #ifdef DEBUG_READ_QUEUE
        printf("Failed to read from SSD, error code: %d\n", read_result);
        #endif

        return nullptr;
    }

    #ifdef DEBUG_READ_QUEUE
    printf("Waiting for SSD to complete read...\n");
    #endif

    while (out_stand) {
        spdk_nvme_qpair_process_completions(qpair, 0);
    }

    #ifdef DEBUG_READ_QUEUE
    printf("SSD read completed.\n");
    #endif

    return buffer;
}


void read_multiple_lbAs(uint64_t start_page_id, uint64_t num_pages) {

    for (uint64_t i = 0; i < num_pages; ++i) {
        uint64_t current_page_id = start_page_id + i;
        char* buffer = read_queue(current_page_id);

        if (buffer == nullptr) {
            
            logger.log(nexio_logger::error, "Failed to read page " + std::to_string(current_page_id));
            continue; 
        }

         char* value_in_page = new char[value_size];
         size_t count = 0; // 用于跟踪每行输出的值的数量
        for (size_t offset = 0; offset < 128*512; offset += value_size) {
            memcpy(value_in_page, buffer+offset, value_size);
            uint64_t value = big_endian2little_endian(value_in_page, KEY_SIZE);

            std::cout << value <<" ";
            if (++count % 20 == 0) {
                std::cout << std::endl;
            }
        }
        if (count % 20 != 0) {
            std::cout << std::endl; // 如果最后一行不足20个值，也添加换行符
        }

        printf("\n::::::::next:::::::::::\n");
        
        spdk_dma_free(buffer);

        #ifdef DEBUG_READ_QUEUE
        printf("Finished processing page: %lu\n", current_page_id);
        #endif
    }


}






void   printBlockInformation() 
{
    for (const auto& element : block_information) 
	{
        uint64_t key = element.first;
        std::pair<int,int> value = element.second;
		if(value.first!=0 && value.second!=0)
		{
			if(value.second/value.first >= 0.9) 
			{
				// printf("Key: %lu, Value: %.3f\n", key, (float)value.second/value.first);
			}
		}
    }
}
