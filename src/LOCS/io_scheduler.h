#include "global_variables.h"
#include <spdk/nvme_ocssd.h>


#ifndef LOCS_IO_SCHEDULER_H
#define LOCS_IO_SCHEDULER_H

/* I/O queues */
extern struct channels_io *channels;

extern uint16_t current_channel;

/* create I/O queues for multi-channel OCSSD */
int create_queue();


/* Insert I/O request into appropriate queue */
int insert_queue();


/* select appropriate I/O queue and  */
int select_queue(int mode);


#endif //LOCS_IO_SCHEDULER_H