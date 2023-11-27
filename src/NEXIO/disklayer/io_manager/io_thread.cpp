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
#include "disklayer/io_manager.h"

std::queue<write_task_paramaters*> io_tasks;

std::queue<separation_write_task_paramaters*> separation_io_tasks;

pthread_mutex_t io_mutex;
pthread_cond_t io_cond;
volatile bool keep_running;
pthread_t io_thread;



int init_mutex() 
{
    if (pthread_mutex_init(&io_mutex, NULL) != 0) {
        fprintf(stdout,"\033[0;31m[ERROR]\033[0m Failed to initialize io_mutex.\n");
        return -1;
    } else {
        fprintf(stdout, "\033[1;32m[INFO]\033[0m io_mutex initialized successfully.\n");

    }

    if (pthread_cond_init(&io_cond, NULL) != 0) {
        fprintf(stdout, "\033[0;31m[ERROR]\033[0m Failed to initialize io_cond.\n");  
        return -1;
    } else {
        fprintf(stdout, "\033[1;32m[INFO]\033[0m io_cond initialized successfully.\n");
    }

    keep_running = true;

    return 0; // Make sure to return 0 if everything is successful
}

void add_io_task(write_task_paramaters* task) {
    pthread_mutex_lock(&io_mutex);
    io_tasks.push(task);
    fprintf(stderr, "\033[1;33m[INFO]\033[0m Added a new regular I/O task.                 \n");
    fflush(stderr);
    pthread_cond_signal(&io_cond);
    fprintf(stderr, "\033[1;33m[INFO]\033[0m Signaled io_cond after adding a regular I/O task.\n");
    fflush(stderr);
    pthread_mutex_unlock(&io_mutex);
}

void add_separation_io_task(separation_write_task_paramaters* task) {
    pthread_mutex_lock(&io_mutex);
    separation_io_tasks.push(task);
    // fprintf(stderr, "\033[1;33m[INFO]\033[0m Added a new separation I/O task.                 \n");
    pthread_cond_signal(&io_cond);
    // fprintf(stderr, "\033[1;33m[INFO]\033[0m Signaled io_cond after adding a separation I/O task.\n");
    pthread_mutex_unlock(&io_mutex);
}







write_task_paramaters* create_io_task(void* task_buffer) 
{
    write_task_paramaters* task = (write_task_paramaters*)malloc(sizeof(write_task_paramaters));
    if (!task) {
        return nullptr;
    }
    task->lba_start = my_controller.current_write_lba_num;
    task->lba_count = my_controller.nexio_write_uint;
    task->taskType = IOTaskType::NON_SEPARATION_WRITE_TASK;
    task->buffer = task_buffer;
    return task;
}



separation_write_task_paramaters* create_separation_io_task(void* task_buffer)
{
    separation_write_task_paramaters* task = (separation_write_task_paramaters*)malloc(sizeof(separation_write_task_paramaters));
    if (!task) {
        logger.log(nexio_logger::error, "Failed to allocate memory for separation_write_task_paramaters.");
        return nullptr; 
    }

    task->buffer = task_buffer;
    task->taskType = IOTaskType::SEPARATION_KEY_WRITE_TASK;
    return task;
}

separation_write_task_paramaters* create_separation_io_task()
{
    separation_write_task_paramaters* task = (separation_write_task_paramaters*)malloc(sizeof(separation_write_task_paramaters));
    if (!task) {
        logger.log(nexio_logger::error, "Failed to allocate memory for separation_write_task_paramaters.");
        return nullptr; 
    }
    return task;
}


void* io_management(void* arg) 
{
    try {
        while (true) {
            pthread_mutex_lock(&io_mutex);
            while (io_tasks.empty() && separation_io_tasks.empty()) {
                if(!keep_running) { 
                    pthread_mutex_unlock(&io_mutex); 
                    return NULL;
                }
                pthread_cond_wait(&io_cond, &io_mutex); 
                // fprintf(stderr, "\033[0;33m[WAIT]\033[0m Waiting for an I/O task.\n");
            }

            if (!io_tasks.empty()) {
                write_task_paramaters* task = io_tasks.front();
                io_tasks.pop();
                if(task->taskType == IOTaskType::NON_SEPARATION_WRITE_TASK || task->taskType == IOTaskType::NON_SEPARATION_LOG_TASK)
                {
                    if(task->taskType == IOTaskType::NON_SEPARATION_WRITE_TASK) {
                        fprintf(stderr,"\033[1;33m[INFO]\033[0m Popped a regular NON-SEPARATION WRITE I/O task for processing.\n");
                    } else if(task->taskType == IOTaskType::NON_SEPARATION_LOG_TASK) {
                        fprintf(stderr,"\033[1;33m[INFO]\033[0m Popped a regular NON-SEPARATION LOG I/O task for processing.\n");

                    }
                    write_queue(task->lba_start, task->lba_count, task->buffer);
                    spdk_dma_free(task->buffer);
                    free(task);
                    fprintf(stderr, "\033[1;33m[INFO]\033[0m Freed memory for the buffer of the processed regular task.\n");
                }
            }
            else if (!separation_io_tasks.empty()) {
                separation_write_task_paramaters* sep_task = separation_io_tasks.front();
                separation_io_tasks.pop();
                if(sep_task->taskType == IOTaskType::SEPARATION_KEY_WRITE_TASK || sep_task->taskType == IOTaskType::SEPARATION_VAL_WRITE_TASK)
                {
                    // fprintf(stderr,"\033[1;33m[INFO]\033[0m Popped a separation I/O task (DATA) for processing.\n");
                    kv_write_queue((char*)sep_task->buffer, sep_task->block_id, sep_task->mode);
                    spdk_dma_free(sep_task->buffer);
                    free(sep_task);
                }
                else if(sep_task->taskType == IOTaskType::SEPARATION_LOG_TASK)
                {
                    // fprintf(stderr, "\033[1;33m[INFO]\033[0m Popped a separation I/O task (LOG) for processing.\n");
                    kv_log_queue((char*)sep_task->buffer, sep_task->block_id);
                    spdk_dma_free(sep_task->buffer);
                    free(sep_task);
                }

                // fprintf(stderr,"\033[1;33m[INFO]\033[0m Freed memory for the buffer of the processed separation task.\n");
            }
            pthread_mutex_unlock(&io_mutex);
        }
    }
    catch (const std::exception& e) {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Exception in io_management: %s\n", e.what());
        pthread_mutex_unlock(&io_mutex); 
    } catch (...) {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Unknown exception in io_management\n");
        pthread_mutex_unlock(&io_mutex); 
    }
    return NULL;
}

