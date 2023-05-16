#include "io_scheduler.h"
#include <iostream>
#include <queue>
#include "global_variables.h"

struct channels_io *channels = nullptr;
uint64_t current_channel = 0;
uint64_t write_count = 0;
uint64_t last_written_block = 0;
int outstanding_commands = 0;
int erase_outstanding_commands = 0;
uint64_t counts = 0;
std::mutex mtx;
std::map<int, ThreadInfo> thread_map;

int create_threads() 
{
	int err = create_queue();
	if(err != 0)
	{
		printf("create_queue failed!\n");
		return -1;
	}
	printf("core count: %d\n",spdk_env_get_core_count());
	uint32_t main_core = spdk_env_get_current_core();
	printf("main core: %d\n",main_core);

	struct spdk_cpuset	*tmp_cpumask;
	tmp_cpumask = spdk_cpuset_alloc();
	spdk_cpuset_zero(tmp_cpumask);
	spdk_cpuset_set_cpu(tmp_cpumask, 0, true);

	

	struct spdk_thread *thread = spdk_thread_create(NULL, NULL);

    // if (!thread) 
	// {
    //     fprintf(stderr, "failed to create spdk thread\n");
    //     exit(-1);
    // }

    for (int i = 0; i < 1; i++) 
	{
     
			char thread_name[32];
    		snprintf(thread_name, sizeof(thread_name), "thread_%d", i);
        
		
	// 	ThreadInfo info = {thread,channels[i].qpair};
    //     thread_map[i] = info;
    }

	return 0;
}


int create_queue()
{

	uint16_t nchannels = geometry.num_grp; 
	channels = new struct channels_io[nchannels];
	for(uint64_t i = 0;i<nchannels;i++ )
	{
		struct channels_io channel;
		channel.channel_id = i;
		if(ctrlr == NULL)
		{
			printf("ctrlr is NULL!\n");
			return -1;
		}
		channel.qpair = spdk_nvme_ctrlr_alloc_io_qpair(ctrlr, NULL, 0);
		if (channel.qpair == NULL) 
		{
			printf("Failed to allocate I/O qpair for channel %lu\n", i);
			return -1;  
		}
		channel.read_count = 0;
		channel.write_count = 0;
		channel.LWQL = 0;
		channel.current_writer_point = i*geometry.num_pu*geometry.num_chk*geometry.clba;
		channel.current_request_num = 0;
		channels[i] = channel;

		channels[i].chunk_type = new uint8_t[geometry.num_chk*geometry.num_pu];
		memset(channels[i].chunk_type, FREE_CHUNK, sizeof(uint8_t));

		channels[i].all_chunk_count = geometry.num_chk*geometry.num_pu;
		channels[i].used_chunk = 0;
	}

	printf("---- I/O Queues Initialized Successfully!\n");

	return 0;

}

void erase_complete(void *arg, const struct spdk_nvme_cpl *completion)
{
    if (spdk_nvme_cpl_is_error(completion)) 
	{
        printf("OCSSD vector write failed with status 0x%x\n", completion->status.sct);
    }
	printf("Erase completed!\n");
	erase_outstanding_commands--;
	
}

int insert_erase_queue(uint64_t chunk_id, spdk_ocssd_chunk_information_entry *chunk_info)
{
	uint64_t channel_id = chunk_id / (geometry.num_pu*geometry.num_chk);
	if(channels[channel_id].chunk_type[chunk_id - (channel_id*geometry.num_chk*geometry.num_pu)] == FREE_CHUNK)
	{
		printf("Chunk is already free!\n");
		return -1;
	}
	uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t), 0x10, NULL);
	lbalist[0] = chunk_id * geometry.clba;
	erase_outstanding_commands = 0;

	if(spdk_nvme_ocssd_ns_cmd_vector_reset(ns, channels[channel_id].qpair,lbalist,1,chunk_info,erase_complete, NULL) == 0) 
	{
		// channels[channel_id].current_request_num++;
		erase_outstanding_commands++;
	}
	else
	{
		printf("Failed to submit erase request!\n");
		return -1;
	}

	while (erase_outstanding_commands)
	{
		spdk_nvme_qpair_process_completions(channels[channel_id].qpair, 0);
	}
	
	channels[channel_id].chunk_type[chunk_id - (channel_id*geometry.num_chk*geometry.num_pu)] = FREE_CHUNK;

	channels[channel_id].used_chunk--;

	spdk_dma_free(lbalist);

	return 0;
}

int insert_erase_queue(uint64_t chunk_id)
{
	uint64_t channel_id = chunk_id / (geometry.num_pu*geometry.num_chk);

	uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t), 0x10, NULL);
	lbalist[0] = chunk_id;

	spdk_ocssd_chunk_information_entry *erased_chunk_info =  (spdk_ocssd_chunk_information_entry *)spdk_dma_malloc(sizeof(spdk_ocssd_chunk_information_entry), 0x40, NULL);

	erased_chunk_info->cnlb = 4096;
	erased_chunk_info->wp = 4096;
	erased_chunk_info->slba = chunk_id *4096;
	erased_chunk_info->wli = chunk_id;


	if(spdk_nvme_ocssd_ns_cmd_vector_reset(ns, channels[channel_id].qpair,lbalist,1,erased_chunk_info,erase_complete, NULL) == 0) 
	{
		channels[channel_id].current_request_num++;
	}
	else
	{
		printf("Failed to submit erase request!\n");
		return -1;
	}

	while (channels[channel_id].current_request_num)
	{
		spdk_nvme_qpair_process_completions(channels[channel_id].qpair, 0);
	}
	
	channels[channel_id].chunk_type[chunk_id - (channel_id*geometry.num_chk*geometry.num_pu)] = FREE_CHUNK;

	channels[channel_id].used_chunk--;

	spdk_dma_free(lbalist);

	spdk_dma_free(erased_chunk_info);

	return 0;
}




/* *
 * Insert write request into appropriate queue 
 * */

void check_if_erase(uint64_t channel_id)
{
	double free_percent = 1- (double)channels[channel_id].used_chunk / (double)channels[channel_id].all_chunk_count;
	while(free_percent < 0.2)
	{
		printf("		Channel %lu is full, start to erase!\n", channel_id);
		uint64_t chunk_id = 0;
		for(uint64_t i = 0;i<geometry.num_chk*geometry.num_pu;i++)
		{
			
			if(channels[channel_id].chunk_type[i] == DISUSED_CHUNK)
			{
				chunk_id = i+(channel_id*geometry.num_pu*geometry.num_chk);
				printf("		Chunk %lu is disused, start to erase!\n", chunk_id);
				insert_erase_queue(chunk_id);
			}
		}
		free_percent = (double)channels[channel_id].used_chunk / (double)channels[channel_id].all_chunk_count;
		printf("		Channel %lu is %lf percent free!\n", channel_id, free_percent);
	}
}


void write_complete(void *arg, const struct spdk_nvme_cpl *completion)
{
    if (spdk_nvme_cpl_is_error(completion)) 
	{
        printf("OCSSD vector write failed with status 0x%x\n", completion->status.sct);
    }

	uint64_t *channel_id_ptr = (uint64_t *)arg;
    uint64_t channel_id = *channel_id_ptr;
	channels[channel_id].current_request_num--;  
}

uint64_t big_to_little_endian(const char *buffer, size_t size) 
{
    uint64_t result = 0;
    for (size_t i = 0; i < size; ++i) 
	{
        result |= (static_cast<uint64_t>(static_cast<unsigned char>(buffer[i])) << (8 * (size - 1 - i)));
    }
    return result;
}

uint64_t test1(const char* buffer)
{
    uint64_t result = 0;
    for (size_t j = 0; j < 8; ++j) 
    {
        result |= (static_cast<uint64_t>(static_cast<unsigned char>(buffer[KEY_SIZE-1-j])) << (8 * j));
    }
    return result;
}

void write_cmd(void *ctx)
{
    struct WriteArgs *args = (struct WriteArgs *)ctx;

    if (spdk_nvme_ocssd_ns_cmd_vector_write(args->ns, args->qpair, args->buffer, args->lbalist, args->lba_count, args->cb_fn, args->cb_arg, args->io_flags) != 0) 
	{
        fprintf(stderr, "Failed to write data to OCSSD!\n");
    }

    free(args);
}


int insert_write_queue(entry_t* data, uint64_t channel_id, size_t start, size_t end, char *buffer, uint64_t *lbalist)
{
    // Fill the buffer
    for (size_t i = start, j = 0; i < end; i++)
    {
        memcpy(buffer + j, data[i].key, KEY_SIZE);
        j += KEY_SIZE;

        memcpy(buffer + j, data[i].val, VAL_SIZE);
        j += VAL_SIZE;
    }

    // Fill lbalist
    for(uint32_t j = 0;j<SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES;j++)
    {
        lbalist[j] = channels[channel_id].current_writer_point;
        channels[channel_id].current_writer_point++;
    }

	struct WriteArgs *args = (struct WriteArgs *)malloc(sizeof(struct WriteArgs));
    if (!args) 
	{
        fprintf(stderr, "Failed to allocate WriteArgs\n");
        return -1;
    }

    args->ns = ns;
    args->qpair = thread_map[channel_id].qpair;
    args->buffer = buffer;
    args->lbalist = lbalist;
    args->lba_count = 64;
    args->cb_fn = write_complete;
    args->cb_arg = &channel_id;
    args->io_flags = 0;

    spdk_thread_send_msg(thread_map[channel_id].thread, write_cmd, args);


	while (channels[channel_id].current_request_num) 
	{
		// printf("Waiting for outstanding_commands: %d\n", outstanding_commands);
		spdk_nvme_qpair_process_completions(channels[channel_id].qpair, 0);
	}
	
	channels[channel_id].write_count++;
	channels[channel_id].LWQL += 7500 * 64;

	// std::cout << "Leaving insert_write_queue" << std::endl;

	return 0;

}


int select_write_queue(entry_t* data, size_t data_size, int mode, uint64_t& last_written_block_temp)
{

	std::cout << "Entering select_write_queue with data size = " << data_size << std::endl;

    if(mode == OCSSD_WRITE)
    {
        mtx.lock();  
        uint64_t channel_id = current_channel;
        current_channel = (current_channel + 1) % geometry.num_grp;
        mtx.unlock();  

		size_t offset_of_vector = SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES*page_size/(sizeof(entry_t));

		char *buffer = (char *)spdk_dma_malloc(offset_of_vector * sizeof(entry_t), 0x1000, NULL);
        uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t) *SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES , 0x1000, NULL);

		for (size_t i = 0; i < data_size; i += offset_of_vector)
        {
            size_t end = std::min(i + offset_of_vector, data_size);
            // insert_write_queue(data, channel_id, i, end, buffer, lbalist);
        }

        // Free buffer and lbalist after all insert_write_queue calls
        spdk_dma_free(buffer);
        spdk_dma_free(lbalist);

		std::cout << "Leaving select_write_queue\n" << std::endl;
	
		// if(write_count % geometry.clba == 0 && write_count != 0)
		// {
		// 	last_written_block = (channels[channel_id].current_writer_point/geometry.clba) -1;
		// 	channels[channel_id].used_chunk++;
		// 	channels[channel_id].chunk_type[last_written_block-(channel_id*geometry.num_chk*geometry.num_pu)] = DATA_CHUNK;
			
		// 	if(channels[channel_id].current_writer_point == (channel_id+1)*(geometry.num_chk*geometry.num_pu*geometry.clba))
		// 	{   
		// 		channels[channel_id].current_writer_point = channel_id*geometry.num_pu*geometry.num_chk*geometry.clba;
		// 	}
		// 	last_written_block_temp = last_written_block;
		// }
		// else
		// {
		// 	last_written_block_temp = last_written_block;
		// }

    }
    else if(mode == OCSSD_ERASE)
    {
        mtx.lock();  // 获取锁
        current_channel = (current_channel + 1) % geometry.num_grp;
        mtx.unlock();  // 释放锁
    }

    return 0;
}


int insert_write_queue(std::vector<entry_t>& data, uint64_t channel_id)
{
	char *buffer = (char *)spdk_dma_malloc(data.size() *sizeof(entry_t), 0x1000, NULL);

	for(uint64_t i = 0,j=0;i<data.size();i++)
	{
		memcpy(buffer + j, data[i].key, KEY_SIZE);
		j += KEY_SIZE;

		memcpy(buffer + j, data[i].val, VAL_SIZE);
        j += VAL_SIZE;
	}
	
	uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t) * 64, 0x1000, NULL);
	for(uint32_t j = 0;j<64;j++)
	{
		lbalist[j] = channels[channel_id].current_writer_point;
		channels[channel_id].current_writer_point++;
	}
	if (spdk_nvme_ocssd_ns_cmd_vector_write(ns,channels[channel_id].qpair,(void *)buffer,lbalist,64,write_complete, NULL,0) == 0)
	{
		channels[channel_id].current_request_num++;
		write_count += 64;
	}
	else
	{
		printf("Failed to write data to OCSSD!\n");
		return -1;
	}

	while (channels[channel_id].current_request_num) 
	{
		// printf("Waiting for outstanding_commands: %d\n", outstanding_commands);
		spdk_nvme_qpair_process_completions(channels[channel_id].qpair, 0);
	}

	spdk_dma_free(buffer);
	spdk_dma_free(lbalist);	
	channels[channel_id].write_count++;
	channels[channel_id].LWQL += 7500 * 64;

	return 0;
}

int select_write_queue(std::vector<entry_t>& data, int mode)
{
	if(mode == OCSSD_WRITE)
	{
		mtx.lock();  
		uint64_t channel_id = current_channel;
		current_channel = (current_channel + 1) % geometry.num_grp;
		mtx.unlock();  
		insert_write_queue(data, channel_id);

		if(write_count % geometry.clba == 0 && write_count != 0)
		{
			last_written_block = (channels[current_channel].current_writer_point/geometry.clba) -1;
			channels[current_channel].used_chunk++;
			channels[current_channel].chunk_type[last_written_block-(current_channel*geometry.num_chk*geometry.num_pu)] = DATA_CHUNK;
			
			if(channels[current_channel].current_writer_point == (current_channel+1)*(geometry.num_chk*geometry.num_pu*geometry.clba))
			{	
				channels[current_channel].current_writer_point = current_channel*geometry.num_pu*geometry.num_chk*geometry.clba;
			}
		}
	}
	else if(mode == OCSSD_ERASE)
	{
		mtx.lock();  // 获取锁
		current_channel = (current_channel + 1) % geometry.num_grp;
		mtx.unlock();  // 释放锁
	}

	return 0;
}


/* *
 * Insert read request into appropriate queue 
 * */
void read_complete(void *arg, const struct spdk_nvme_cpl *completion)
{
    if (spdk_nvme_cpl_is_error(completion)) 
	{
        printf("OCSSD vector write failed with status 0x%x\n", completion->status.sct);
    }
	outstanding_commands--;  
}


char* insert_read_queue(uint64_t start_address)
{

	char *buffer = (char *)spdk_dma_malloc(page_size*64 , 0x40000, NULL);
	uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t) * 64, 0x1000, NULL);
	uint64_t channel_id = start_address / (geometry.num_grp*geometry.num_pu*geometry.num_chk*geometry.clba);

	for(uint64_t j =0;j<64;j++)
	{
		lbalist[j] = start_address+j;
	}

	if (spdk_nvme_ocssd_ns_cmd_vector_read(ns,channels[channel_id].qpair,(void *)buffer,lbalist,64,read_complete, NULL,0) == 0)
	{
		outstanding_commands++;
	}
	else
	{
		printf("Failed to write data to OCSSD!\n");
		return nullptr;
	}

	while (outstanding_commands) 
	{
		// printf("Waiting for outstanding_commands: %d\n", outstanding_commands);
		spdk_nvme_qpair_process_completions(channels[channel_id].qpair, 0);
	}

	spdk_dma_free(lbalist);	
	
	channels[channel_id].read_count++;
	channels[channel_id].LWQL += 7500 * 4096;
	return buffer;
}


std::vector<entry_t> select_read_queue(uint64_t start_address, int mode)
{
	counts = 0;
	std::vector<entry_t> data;
	char *temp_data;
	entry_t entry;
	if(mode == OCSSD_READ)
	{
		temp_data = insert_read_queue(start_address);
		
		for(uint64_t j=0;j<page_size*64;)
		{
			memcpy(entry.key, temp_data + j, KEY_SIZE);
        	j += KEY_SIZE;
			memcpy(entry.val, temp_data + j, VAL_SIZE);
        	j += VAL_SIZE;
			data.push_back(entry);

#ifdef BIG_TO_LITTLE_ENDIAN
			if(j >= 262144)
			{
				printf("read test: key = %lu  val=%lu \n", test1(entry.key),  test1(entry.val));
			}
			if(counts < 1)
			{
				printf("read test: key = %lu  val=%lu \n", test1(entry.key),  test1(entry.val));
			}
			counts++;
#endif
		}
	}
	
	// printf("key = %lu  val=%lu \n", data[0].key,  data[0].val);
	// printf("key = %lu  val=%lu \n", data[10].key, data[11].val);
	// printf("key = %lu  val=%lu \n", data[data.size()-1].key, data[data.size()-1].val);

	spdk_dma_free(temp_data);
	return data;
}
