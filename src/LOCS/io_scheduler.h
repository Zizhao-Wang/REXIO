#include "global_variables.h"
#include <pthread.h>
#include <vector>
#include <mutex>
#include <map>
#include <spdk/nvme_ocssd.h>
#include "spdk/thread.h"

#ifndef LOCS_IO_SCHEDULER_H
#define LOCS_IO_SCHEDULER_H

/* I/O queues */

extern std::mutex mtx;

extern uint64_t count;

extern uint64_t write_count;

extern uint64_t time_record;

extern uint64_t time_record2;

extern uint64_t current_channel;

extern int outstanding_commands;

extern uint64_t last_written_block;

extern struct channels_io *channels;

extern int erase_outstanding_commands;


/* *
 *  I/O Thread information and thread parameters  
 * */
struct ThreadInfo 
{
    spdk_thread *thread;
    spdk_nvme_qpair *qpair;

};

struct WriteArgs 
{
    struct spdk_nvme_ns *ns;
    struct spdk_nvme_qpair *qpair;
    void *buffer;
    uint64_t *lbalist;
    uint32_t lba_count;
    spdk_nvme_cmd_cb cb_fn;
    void *cb_arg;
    uint32_t io_flags;
};

extern std::map<int, ThreadInfo> thread_map;


/* *
 * ================= create I/O queues for multi-channel OCSSD  ==================== 
 * */
int create_queue();

int create_threads();



/* *
 * ================= Insert erase request into appropriate queue  ==================== 
 * */
int insert_erase_queue(uint64_t chunk_id, spdk_ocssd_chunk_information_entry *chunk_info);

int insert_erase_queue(uint64_t chunk_id);



/* *
 *  ================= Insert write request into appropriate queue ==================== 
 * */
void check_if_erase();

int insert_write_queue(entry_t* data, uint64_t channel_id, size_t start, size_t end, char *buffer, uint64_t *lbalist);

int select_write_queue(entry_t* data, size_t data_size, int mode);

int select_write_queue(std::vector<entry_t>& data, int mode);



/* *
 * ================= Insert read request into appropriate queue ====================  
 * */
char* insert_read_queue(uint64_t start_address);

std::vector<entry_t> select_read_queue(uint64_t start_address, int mode);



#endif //LOCS_IO_SCHEDULER_H