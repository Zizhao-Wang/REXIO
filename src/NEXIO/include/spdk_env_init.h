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
extern uint64_t sectors_per_page;

extern struct spdk_nvme_ns * ns;

extern struct spdk_env_opts opts;  //environment options

extern struct spdk_nvme_ctrlr *ctrlr; // pointer that point to the controller

extern struct spdk_nvme_transport_id trid; //transport id
 
extern struct spdk_nvme_detach_ctx *g_detach_ctx; //detach context



struct nvme_ns_info {
    struct spdk_nvme_ns *ns;
    uint32_t ns_id;
    uint32_t sector_size;   // sector size in SPDK, sector size may equals lba size
    uint32_t lba_size;      // logical block size in SPDK
    uint32_t ns_lba_count; // number of logical blocks in SPDK
};


/**
 * \brief Structure to hold information of multiple NVMe namespaces.
 */
struct nvme_device_info {
    struct spdk_nvme_ctrlr *ctrlr;
    struct nvme_ns_info *ns_info_array;
    uint32_t num_ns;  
};

extern struct nvme_device_info *device_info; // pointer that point to the device info




#ifdef __OCSSD__CONTROLLER__

/**
 * \brief Print the geometry details of an OCSSD.
 * 
 * This function displays the geometry information of an Open-Channel Solid State Drive (OCSSD),
 * including LBA format, media and controller capabilities, wear-level index delta threshold, 
 * and other OCSSD-specific details.
 * 
 * \param geometry_data Pointer to the OCSSD's geometry data structure.
 */
void print_ocssd_geometry(struct spdk_ocssd_geometry_data *geometry_data);

#elif defined(NVME_SSD_CONTROLLER)

/**
 * \brief Print the geometry details of a standard NVMe SSD.
 * 
 * This function displays the geometry and other relevant information of a standard 
 * NVMe Solid State Drive (SSD), such as model number, serial number, firmware version, 
 * and details about each namespace.
 * 
 * \param ctrlr Pointer to the NVMe controller structure.
 */
void print_nvme_geometry(struct spdk_nvme_ctrlr *ctrlr);

#endif

#ifdef __OCSSD__CONTROLLER__

/**
 * \brief Callback function after retrieving OCSSD geometry.
 * 
 * This function is called once the OCSSD's geometry data is fetched. It processes 
 * the completion of the geometry retrieval command, handling any errors and 
 * possibly updating global state based on the returned data.
 * 
 * \param cb_arg Callback argument, possibly pointing to data structures to be updated.
 * \param cpl Completion structure with details of the command's completion.
 */
void get_ocssd_geometry_completion(void *cb_arg, const struct spdk_nvme_cpl *cpl);


/**
 * \brief Initializes the environment for NVMe OCSSD operations.
 * 
 * This function sets up the necessary environment to prepare for NVMe OCSSD operations. 
 * It may include initializing controllers, connecting to the device, setting up global 
 * variables, and more.
 * 
 * \return 0 on success, non-zero on failure.
 */
int nvme_ocssd_environment_init();

#elif defined(NVME_SSD_CONTROLLER)

/**
 * \brief Initializes global variables for NVMe SSD operations.
 * 
 * This function sets up global variables that will be used in subsequent NVMe SSD operations.
 * These might include pointers, sizes, flags, or other relevant data.
 * 
 * \return 0 on success, non-zero on failure.
 */
int variables_init();

/**
 * \brief Initializes the nvme_device_info structure with controller and namespaces data.
 * 
 * This function initializes the nvme_device_info structure. It gathers information about 
 * each namespace associated with the provided controller, including ID, page size, and block size.
 * The gathered information is stored in the ns_info_array.
 * 
 * \param ctrlr Pointer to the NVMe controller structure.
 * \return Pointer to the initialized nvme_device_info structure, or NULL on failure.
 */
struct nvme_device_info* initialize_ns_info(struct spdk_nvme_ctrlr *ctrlr);

/**
 * \brief Initializes the environment for standard NVMe SSD operations.
 * 
 * This function sets up the necessary environment to prepare for standard NVMe SSD operations.
 * It may include initializing controllers, connecting to the device, setting up global 
 * variables, and more.
 * 
 * \return 0 on success, non-zero on failure.
 */
int nvme_ssd_environment_init();


/**
 * \brief Frees the memory allocated for the nvme_device_info structure.
 * 
 * This function releases the memory dynamically allocated for the nvme_device_info structure.
 * It ensures that memory associated with each namespace and the controller is also freed 
 * to prevent memory leaks.
 * 
 * \param device_info Pointer to the nvme_device_info structure that needs to be freed.
 */
void free_nvme_device_info();




#endif


#endif // SPDK_ENV_INIT_H