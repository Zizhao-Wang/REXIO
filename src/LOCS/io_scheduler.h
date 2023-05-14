#include "global_variables.h"
#include <pthread.h>
#include <vector>
#include <mutex>
#include <spdk/nvme_ocssd.h>

#ifndef LOCS_IO_SCHEDULER_H
#define LOCS_IO_SCHEDULER_H

/* I/O queues */

extern std::mutex mtx;

extern struct channels_io *channels;

extern uint64_t write_count;

extern uint64_t current_channel;

extern uint64_t last_written_block;

extern uint64_t count;





/* create I/O queues for multi-channel OCSSD */
int create_queue();

extern int outstanding_commands;
extern int erase_outstanding_commands;

/* *
 * Insert erase request into appropriate queue 
 * */
int insert_erase_queue(uint64_t chunk_id, spdk_ocssd_chunk_information_entry *chunk_info);

int insert_erase_queue(uint64_t chunk_id);


/* *
 * Insert write request into appropriate queue 
 * */

int select_write_queue(std::vector<entry_t>& data, int mode);

int insert_write_queue(std::vector<entry_t>& data, uint64_t channel_id, size_t start, size_t end, char *buffer, uint64_t *lbalist);

/* *
 * Insert read request into appropriate queue 
 * */
char* insert_read_queue(uint64_t start_address);

int select_write_queue(std::vector<entry_t>& data, int mode, uint64_t& last_written_block_temp);

std::vector<entry_t> select_read_queue(uint64_t start_address, int mode);



#endif //LOCS_IO_SCHEDULER_H