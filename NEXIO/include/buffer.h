/**
 * @file buffer.h
 * @date  10/10/2023
 * @author zz.wang
 * @brief Header for write buffer management.
 *
 * This header defines the structure and operations for managing the buffers
 * used in the NEXIO project. The buffer allows asynchronous writing operations
 * to enhance performance by batching multiple writes.
 *
 * @note Ensure to initialize the buffer before any operations and free resources after use.
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <spdk/env.h>
#include <spdk/nvme.h>
#include <spdk/log.h>
#include <spdk/bdev.h>
#include <spdk/json.h>
#include <spdk/trace.h>
#include <spdk/event.h>




#endif // BUFFER_H
