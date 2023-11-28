#include "io_manager.h"
#include "memlayer/syncstore.h"
#include "write_buffer.h"
#include "include/buffer.h"
#include "include/spdk_env_init.h"
#include "include/log.h"


int out_stand = 0;

size_t num_data_page = 0;;

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
    total_write_bytes += context->io_size;
    delete context;  

    return ;
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
            logger.log(nexio_logger::info, "Successfully sent write request to OCSSD. LBA Start: " + std::to_string(lba_start) + ", LBA Count: " + std::to_string(lba_count), "key");
        }
        else if(mode == NVME_SSD_DATA_VALUE_WRITE){
            logger.log(nexio_logger::info, "Successfully sent write request to OCSSD. LBA Start: " + std::to_string(lba_start) + ", LBA Count: " + std::to_string(lba_count),"val");
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
    if (spdk_nvme_cpl_is_error(completion)) {
        logger.log(nexio_logger::error, "OCSSD log write failed with status 0x" + std::to_string(completion->status.sct));
        return;
    }
    out_stand--;
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
	
	// uint64_t current_writer_pointer = BlockWritePointers[block_id] + block_id*geometry.clba;
	
	// printf("current_writer_pointer:%lu BlockWritePointers[block_id]:%lu block_id:%lu in kv_log_queue\n", current_writer_pointer,BlockWritePointers[block_id],block_id);
#ifdef IO_DEBUG
	// if(block_id)
	// {
		// printf("current_writer_pointer:%lu BlockWritePointers[block_id]:%lu block_id:%lu in kv_log_queue\n", current_writer_pointer,BlockWritePointers[block_id],block_id);
	// }
#endif
    uint64_t lba_start = 0;
	uint64_t lba_count = 0;

    if (spdk_nvme_ns_cmd_write(ns, qpair, write_buffer, lba_start, lba_count, log_write_complete, NULL, 0) == 0) {
    	out_stand++;
	} else {
    	logger.log(nexio_logger::error, "Failed to write data to OCSSD!");
    	return -1;
	}


    while (out_stand)
	{
		spdk_nvme_qpair_process_completions(qpair, 0);
	}
	uint64_t a;

	// pointer_Renew(OCSSD_LOG_WRITE_TIOCS, current_writer_pointer,block_id,current_writer_pointer);


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
    char *buffer = (char *)spdk_dma_malloc(sectors_per_page , 0x1000, NULL);
    if(buffer == NULL) {
        logger.log(nexio_logger::error, "Failed to allocate buffer in read_queue!");
        return nullptr;
    }

    uint64_t lba_start = 0;
    uint64_t lba_count = 0;

    if (spdk_nvme_ns_cmd_read(ns, qpair, (void *)buffer, lba_start, lba_count, page_read_complete, NULL, 0) == 0) {
        out_stand++;
        logger.log(nexio_logger::info, "Successfully sent read request to OCSSD.");
    } else {
        logger.log(nexio_logger::error, "Failed to read data from OCSSD!");
        spdk_dma_free(buffer);
        return nullptr;
    }

    while (out_stand) {
        spdk_nvme_qpair_process_completions(qpair, 0);
    }

    return buffer;
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
