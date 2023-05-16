#include <iostream>
#include <vector>
#include <spdk/nvme.h> //  other project's .h files SPDK's .h files 
#include <spdk/env.h>
#include <spdk/log.h>
#include <spdk/nvme_ocssd.h>
#include <spdk/nvme_ocssd_spec.h>
#include "../Auxizilary/pre_definition.h"


#ifndef LOCS_GLOBALVARIABLE_H
#define LOCS_GLOBALVARIABLE_H


/**
 * global variables for define LOCS variables and spdk environment variables
 **/

// global variables for define spdk environment 
extern struct spdk_ocssd_geometry_data geometry;

extern int backend_commands;

extern struct spdk_env_opts opts; //environment options
    
extern struct spdk_nvme_ctrlr *ctrlr; // pointer that point to the controller

extern struct spdk_nvme_transport_id trid; //transport id

extern struct spdk_nvme_ns * ns;

extern std::vector<uint64_t> temp_pointers;
 
extern struct spdk_nvme_detach_ctx *g_detach_ctx; //detach context

// global varibales for define LOCS variables 
extern uint64_t chunk_capacity; // maximum data entries of a chunk

extern uint64_t max_data_entries_per_io; // maximum data entries of a io

extern uint64_t page_size;  // bytes of a page/(sector)

extern struct channels_io *channels;

extern bool geometry_completed;



/**
 *  Mode selector 
 **/

/* Mode selector */
#define OCSSD_WRITE 0

#define OCSSD_READ  1

#define OCSSD_ERASE 2

/* status for check run is empty or not */
#define FULL_EMPTY 0

#define NOT_EMPTY  1

/* mode selector for erase mode */
#define FREE_ERASE_MODE  0

#define FORCE_ERASE_MODE 1


/* chunk type definition */
#define FREE_CHUNK 0

#define DATA_CHUNK 1

#define DISUSED_CHUNK 2


/* *
 *   channel managers 
 * */
struct channels_io
{
	struct spdk_nvme_qpair *qpair;
	uint32_t read_count;
	uint32_t write_count;
	uint16_t channel_id;
	uint64_t LWQL;
	uint64_t current_writer_point; 
	uint64_t current_request_num;
	uint64_t all_chunk_count;
	uint64_t used_chunk;
	uint8_t *chunk_type;  
};



/**
 *  Function declaration
 **/
uint64_t get_max_data_entries_per_io();

/* Function for initialize global variables */
int variables_init();

int environment_init();



#endif //LOCS_GLOBALVARIABLE_H