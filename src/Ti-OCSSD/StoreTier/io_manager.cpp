#include "io_manager.h"
#include "../../LOCS/global_variables.h"
#include "syncstore.h"

int reads = 0;

int writes = 0;

int resets = 0;

int out_stand = 0;

size_t num_data_page = 0;;

uint64_t page_pointer = 0;

struct spdk_nvme_qpair *qpair = nullptr;

std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkLog;

std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkData;

std::unordered_map<uint64_t, uint64_t> BlockWritePointers;


int pointer_Renew(int mode, uint64_t log_pointer = 0)
{

	if(mode == OCSSD_DATA_WRITE_TIOCS && log_pointer == 0)
	{
		BlockWritePointers[page_pointer/geometry.clba]= BlockWritePointers[page_pointer/geometry.clba] + sectors_per_page;
        if(ChunkData[page_pointer/geometry.clba].size()>=num_data_page)
        {
            page_pointer += (geometry.clba-(page_pointer%geometry.clba));
        }
		else
		{
			page_pointer += sectors_per_page;
		}
	}
	else if(mode == OCSSD_LOG_WRITE_TIOCS)
	{
		page_pointer += (log_pointer/geometry.clba == page_pointer/geometry.clba)?sectors_per_page:0;
        BlockWritePointers[log_pointer/geometry.clba]= BlockWritePointers[log_pointer/geometry.clba] + sectors_per_page;
	}
   
    return 0;
}

/**  
 * ================= DATA WRITE(I/O)  module ====================  
 **/

void page_write_complete(void *arg, const struct spdk_nvme_cpl *completion)
{
    if (spdk_nvme_cpl_is_error(completion)) 
	{
        printf("OCSSD vector write failed with status 0x%x\n", completion->status.sct);
		return ;
    }
	writes++;
	out_stand--;
}

int write_queue()
{
	uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t) *sectors_per_page, 0x1000, NULL);
	if(lbalist == NULL)
	{
		printf("Failed to allocate memory for lbalist!\n");
		exit(-1);
	}
	for(uint32_t j = 0; j<sectors_per_page; j++)
	{
		lbalist[j] = page_pointer+j;
	}

	if (spdk_nvme_ocssd_ns_cmd_vector_write(ns,qpair,Pagedata,lbalist,sectors_per_page,page_write_complete, NULL,0) == 0)
	{
		out_stand++;
		// printf("Write data to OCSSD successfully!\n");
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
	

	ChunkData[page_pointer/geometry.clba].emplace_back(page_pointer);
	pointer_Renew(OCSSD_DATA_WRITE_TIOCS);

	spdk_dma_free(lbalist);

	return 0;
}



/**  
 * ================= LOG WRITE(I/O) module ====================  
 **/

void log_write_complete(void *arg, const struct spdk_nvme_cpl *completion)
{
    if (spdk_nvme_cpl_is_error(completion)) 
	{
        printf("OCSSD vector write failed with status 0x%x\n", completion->status.sct);
		return ;
    }

	writes++;
	out_stand--;
	
}

int write_queue(char *write_buffer, uint64_t block_id)
{

	uint64_t current_writer_pointer = BlockWritePointers[block_id] + block_id*geometry.clba;
	// printf("current_writer_pointer:%lu BlockWritePointers[block_id]:%lu block_id:%lu\n", current_writer_pointer,BlockWritePointers[block_id],block_id);

    uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t) *sectors_per_page, 0x1000, NULL);
	if(lbalist == NULL)
	{
		printf("Failed to allocate memory for lbalist!\n");
		exit(-1);
	}

	for(uint32_t j = 0; j<sectors_per_page; j++)
	{
		lbalist[j] = current_writer_pointer+j;
	}


    if (spdk_nvme_ocssd_ns_cmd_vector_write(ns, qpair, write_buffer, lbalist, sectors_per_page, log_write_complete, NULL, 0) == 0)
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

	pointer_Renew(OCSSD_LOG_WRITE_TIOCS, current_writer_pointer);

	spdk_dma_free(lbalist);

    return 0;
}
 



/**  
 * ================= READ(I/O) module ====================  
 **/

void page_read_complete(void *arg, const struct spdk_nvme_cpl *completion)
{
    if (spdk_nvme_cpl_is_error(completion)) 
	{
        printf("OCSSD vector write failed with status 0x%x\n", completion->status.sct);
    }
	reads++;
	out_stand--;
}

TNCEntry* read_queue(uint64_t page_id)
{
	char *buffer = (char *)spdk_dma_malloc(sectors_per_page*geometry.clba , 0x1000, NULL);
	if(buffer == NULL)
	{
		printf("Failed allocated buffer in read_queue!");
		return nullptr;
	}

	uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t) *sectors_per_page , 0x1000, NULL);
	for(uint64_t j=0;j<sectors_per_page ;j++)
	{
		lbalist[j] = page_id+j;
	}

	if (spdk_nvme_ocssd_ns_cmd_vector_read(ns,qpair,(void *)buffer,lbalist,sectors_per_page,page_read_complete, NULL,0) == 0)
	{
		out_stand++;
	}
	else
	{
		printf("Failed to write data to OCSSD!\n");
		return nullptr;
	}

	while (out_stand) 
	{
		spdk_nvme_qpair_process_completions(qpair, 0);
	}

	spdk_dma_free(lbalist);	
	
	return (TNCEntry*)buffer;
}