#include "io_manager.h"
#include "../../LOCS/global_variables.h"
#include "syncstore.h"


int reads = 0;

int writes = 0;

int resets = 0;

int out_stand = 0;

uint64_t page_pointer = 0;

struct spdk_nvme_qpair *qpair = nullptr;


void page_write_complete(void *arg, const struct spdk_nvme_cpl *completion)
{
    if (spdk_nvme_cpl_is_error(completion)) 
	{
        printf("OCSSD vector write failed with status 0x%x\n", completion->status.sct);
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
		lbalist[j] = page_pointer;
		page_pointer++;
	}

	// if (spdk_nvme_ocssd_ns_cmd_vector_write(ns,qpair,Pagedata,lbalist,sectors_per_page,page_write_complete, NULL,0) == 0)
	// {
	// 	out_stand++;
	// 	// printf("Write data to OCSSD successfully!\n");
	// }
	// else
	// {
	// 	printf("Failed to write data to OCSSD!\n");
	// 	return -1;
	// }

	// while (out_stand)
	// {
	// 	spdk_nvme_qpair_process_completions(qpair, 0);
	// }
	

	spdk_dma_free(lbalist);

	return 0;
}


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
	char *buffer = (char *)spdk_dma_malloc(page_size*64 , 0x40000, NULL);
	uint64_t *lbalist = (uint64_t*)spdk_dma_malloc(sizeof(uint64_t) * 64, 0x1000, NULL);

	for(uint64_t j =0;j<sectors_per_page ;j++)
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