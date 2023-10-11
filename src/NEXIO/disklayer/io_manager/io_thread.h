/**
 * @date   10/10/2023
 * @author zz.wang
 * @details
 *         This header file declares the I/O management thread and associated
 *         utilities for handling asynchronous or synchronous I/O tasks.
 * 
 * IDENTIFICATION:
 *         src/io_manager/io_thread.h
 */
#ifndef IO_THREAD_H
#define IO_THREAD_H

#include <pthread.h>
#include <queue>

extern std::queue<int> io_tasks;
extern pthread_mutex_t io_mutex;
extern pthread_cond_t io_cond;

void* io_management(void* arg);
void add_io_task(int task);

#endif // IO_THREAD_H
