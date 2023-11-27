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
#include <stdint.h>
#include "include/buffer.h"
#include "include/node.h"



enum class IOTaskType {
    NON_SEPARATION_WRITE_TASK,
    NON_SEPARATION_LOG_TASK,
    SEPARATION_KEY_WRITE_TASK,
    SEPARATION_VAL_WRITE_TASK,
    SEPARATION_LOG_TASK,
    READ_TASK
};

/**
 * Struct representing parameters for a write task.
 * Contains the task type, starting LBA, count of LBAs to write,
 * and a pointer to the buffer containing the data.
 */
struct write_task_paramaters {
    IOTaskType taskType;        ///< Task type (either regular write or separation write).
    uint64_t lba_start;         ///< Starting Logical Block Address (LBA) for the write operation.
    uint64_t lba_count;         ///< Number of continuous LBAs to be written.
    void* buffer;    ///< Pointer to the buffer containing the data to be written.
};

/**
 * Struct representing parameters for a separation write task.
 * Contains the task type, block ID and a pointer to the buffer containing the data.
 */
struct separation_write_task_paramaters {
    IOTaskType taskType;        ///< Task type (either regular write or separation write).
    int mode;                   ///< Mode of the separation write (either KEY_BLOCK or VALUE_BLOCK).
    uint64_t block_id;         ///< Block ID for the separation write.
    void* buffer;               ///< Pointer to the buffer containing the data to be written.
};




/**
 * A queue to hold pending write tasks.
 * Tasks are added to this queue when a write buffer is full 
 * and are processed by the io_management thread.
 */
extern std::queue<write_task_paramaters*> io_tasks;

/**
 * Mutex to protect access to the io_tasks queue.
 * Ensures that operations on the queue (push, pop) are thread-safe.
 */
extern pthread_mutex_t io_mutex;

/**
 * Condition variable used to signal the io_management thread 
 * when there's a new task in the io_tasks queue.
 */
extern pthread_cond_t io_cond;

extern volatile bool keep_running;

extern pthread_t io_thread;   //< Thread that manages the I/O tasks in the system.




/**
 * Initialize the mutex and condition variables used in the I/O management system.
 * This function should be called during the initialization phase of the application.
 * 
 * @return 0 on successful initialization, non-zero error code otherwise.
 */
int init_mutex();


/**
 * Add a new write task to the io_tasks queue.
 * This function pushes the given task to the queue and signals 
 * the io_management thread that a new task is available.
 *
 * @param task Pointer to the write_task_paramaters struct representing the new task.
 */
void add_io_task(write_task_paramaters* task);


/**
 * Add a new separation write task to the separation_io_tasks queue.
 * This function pushes the given separation task to the queue and signals 
 * the io_management thread that a new separation task is available.
 *
 * @param task Pointer to the separation_write_task_paramaters struct representing the new separation task.
 */
void add_separation_io_task(separation_write_task_paramaters* task);




/**
 * Create an I/O task based on the provided task buffer.
 *
 * This function initializes a write_task_paramaters structure using
 * the given task_buffer and the current controller parameters. It sets
 * the lba_start, lba_count, taskType, and buffer fields of the task.
 * 
 * @param task_buffer Pointer to the buffer that contains the data to be written.
 * @return Pointer to the initialized write_task_paramaters structure, or nullptr if memory allocation fails.
 */
write_task_paramaters* create_io_task(void* task_buffer);




/**
 * Create an I/O task based on the provided task buffer.
 *
 * This function initializes a write_task_paramaters structure using
 * the given task_buffer and the current controller parameters. It sets
 * the lba_start, lba_count, taskType, and buffer fields of the task.
 * 
 * @param task_buffer Pointer to the buffer that contains the data to be written.
 * @return Pointer to the initialized write_task_paramaters structure, or nullptr if memory allocation fails.
 */
separation_write_task_paramaters* create_separation_io_task(void* task_buffer);



/**
 * Function that manages the I/O tasks in the system.
 * This function continuously checks the io_tasks queue and processes 
 * the tasks available in the queue. It waits for new tasks to arrive if the queue is empty.
 *
 * @param arg Argument passed to the thread (if any, otherwise it can be NULL).
 * @return NULL on successful completion.
 */
void* io_management(void* arg);









#endif // IO_THREAD_H
