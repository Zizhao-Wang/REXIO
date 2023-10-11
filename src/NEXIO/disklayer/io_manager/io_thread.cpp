/**
 * @date   10/10/2023
 * @author zz.wang
 * @details
 *         This source file defines the I/O management thread and associated
 *         utilities for processing asynchronous or synchronous I/O tasks.
 * 
 * IDENTIFICATION:
 *         src/io_manager/io_thread.cpp
 */
#include "io_thread.h"
#include "disklayer/write_buffer.h"
#include "include/buffer.h"

std::queue<int> io_tasks;
pthread_mutex_t io_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t io_cond = PTHREAD_COND_INITIALIZER;


void add_io_task(int task) 
{
    pthread_mutex_lock(&io_mutex);
    io_tasks.push(task);
    pthread_cond_signal(&io_cond);
    pthread_mutex_unlock(&io_mutex);
}

/*
 * A thread for managing I/Os
 */
void* io_management(void* arg) 
{
    while (true) {
        pthread_mutex_lock(&io_mutex);

        while (io_tasks.empty()) {
            pthread_cond_wait(&io_cond, &io_mutex);
        }

        // WriteTask* task = io_tasks.front();
        io_tasks.pop();
        pthread_mutex_unlock(&io_mutex);

        // TODO: 使用task->key和task->value进行实际的写操作
        // 假设我们计算出了结果offset5
        // uint32_t offset5 = ...;

        // pthread_mutex_lock(&task->mutex);
        // task->result = offset5;
        // task->done = true;
        // pthread_cond_signal(&task->cond);
        // pthread_mutex_unlock(&task->mutex);
    }

    return NULL;
}

