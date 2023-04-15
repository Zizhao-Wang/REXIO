#include <iostream>
#include <spdk/nvme.h> //  other project's .h files SPDK's .h files 
#include <spdk/env.h>
#include <spdk/log.h>
#include <spdk/nvme_ocssd.h>
#include <spdk/nvme_ocssd_spec.h>

#ifndef LOCS_GLOBALVARIABLE_H
#define LOCS_GLOBALVARIABLE_H


/* global variables for define spdk environment*/
extern struct spdk_ocssd_geometry_data geometry;

extern int backend_commands;

extern struct spdk_env_opts opts; //environment options
    
extern struct spdk_nvme_ctrlr *ctrlr; // pointer that point to the controller

extern struct spdk_nvme_transport_id trid; //transport id

extern struct spdk_nvme_detach_ctx *g_detach_ctx; //detach context



/* global varibales for define LOCS variables */
extern uint32_t chunk_capacity;

extern uint32_t page_size;



/* Mode selector */
#define OCSSD_WRITE 0
#define OCSSD_READ  1
#define OCSSD_ERASE 2


struct channels_io
{
	struct spdk_nvme_qpair *qpair;
	uint32_t read_count;
	uint32_t write_count;
	uint16_t channel_id;
	uint64_t LWQL;   
};

extern struct channels_io *channels;

/* Function for initialize global variables */

extern bool geometry_completed;

int variables_init();

int environment_init();

#endif //LOCS_GLOBALVARIABLE_H