#include "io_scheduler.h"
#include <iostream>
#include <queue>
#include "global_variables.h"

struct channels_io *channels = nullptr;
uint16_t current_channel = 0;

int create_queue()
{

	uint16_t nchannels = geometry.num_grp; 
	channels = new struct channels_io[nchannels];
	for(uint16_t i = 0;i<nchannels;i++ )
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
			printf("Failed to allocate I/O qpair for channel %u\n", i);
			return -1;  
		}
		channel.read_count = 0;
		channel.write_count = 0;
		channel.LWQL = 0;
		channels[i] = channel;
	}

	printf("---- I/O Queues Initialized Successfully!\n");

	return 0;

}


int insert_queue(int channel_id)
{
	uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t) * geometry.ws_opt, 0x1000, NULL);
	// int err = spdk_nvme_ocssd_ns_cmd_vector_write(channels[channel_id].qpair, lbalist, geometry.ws_opt, , cb_fn, cb_arg);
	// if(err != 0)
	// {
	// 	printf("Failed to write data to OCSSD!\n");
	// 	return -1;
	// }
	channels[channel_id].write_count++;
	channels[channel_id].LWQL += 7500 * 4096;
	return 0;
}




int select_queue(int mode)
{
	if(mode == OCSSD_WRITE)
	{
		channels[current_channel].write_count++;
		current_channel = (current_channel + 1) % geometry.num_grp;
	}
	else if(mode == OCSSD_READ)
	{
		channels[current_channel].read_count++;
		current_channel = (current_channel + 1) % geometry.num_grp;
	}
	else if(mode == OCSSD_ERASE)
	{
		current_channel = (current_channel + 1) % geometry.num_grp;
	}
	

	return 0;
}