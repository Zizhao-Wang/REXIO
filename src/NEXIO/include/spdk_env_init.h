/**
 * @file    spdk_env_init.h
 * @date    10/10/2023
 * @author  zz.wang
 * @details
 *          This header file initializes the SPDK environment, 
 *          handling setup and configuration for SPDK operations.
 * 
 * IDENTIFICATION:
 *          src/NEXIO/spdk_env_init.h
 */

#ifndef SPDK_ENV_INIT_H
#define SPDK_ENV_INIT_H

#include <iostream>
#include <vector>
#include <map>
#include "buffer.h"



/**
 * global variables for define LOCS variables and spdk environment variables
 **/

// global variables for define spdk environment 
extern struct spdk_nvme_ns * ns;

extern struct spdk_env_opts opts;  //environment options

extern struct spdk_nvme_ctrlr *ctrlr; // pointer that point to the controller

extern struct spdk_nvme_transport_id trid; //transport id
 
extern struct spdk_nvme_detach_ctx *g_detach_ctx; //detach context


/**
 *  Function declaration
 **/
int variables_init();

int environment_init();



#endif // SPDK_ENV_INIT_H