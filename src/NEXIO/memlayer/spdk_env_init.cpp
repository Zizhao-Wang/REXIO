#include "include/spdk_env_init.h"
#include "include/node.h"

/*Definition of global variables. */

uint64_t sectors_per_page = 4;

struct spdk_env_opts opts; //environment options
    
struct spdk_nvme_ctrlr *ctrlr = nullptr; // pointer that point to the controller

struct spdk_nvme_transport_id trid;

struct spdk_nvme_ns * ns = nullptr;

struct spdk_nvme_detach_ctx *g_detach_ctx = NULL;

uint64_t chunk_capacity;

uint64_t max_data_entries_per_io;

uint64_t page_size;

struct nvme_device_info *device_info = nullptr;

bool geometry_completed = false;


#ifdef __OCSSD__CONTROLLER__
void print_ocssd_geometry(struct spdk_ocssd_geometry_data *geometry_data)
{
	printf("Namespace OCSSD Geometry\n");
	printf("=======================\n");

	if (geometry_data->mjr < 2) 
    {
		printf("Open-Channel Spec version is less than 2.0\n");
		printf("OC version:             maj:%d\n", geometry_data->mjr);
		return ;
	}

	printf("OC version:                     maj:%d min:%d\n", geometry_data->mjr, geometry_data->mnr);
	printf("LBA format:\n");
	printf("  Group bits:                   %d\n", geometry_data->lbaf.grp_len);
	printf("  PU bits:                      %d\n", geometry_data->lbaf.pu_len);
	printf("  Chunk bits:                   %d\n", geometry_data->lbaf.chk_len);
	printf("  Logical block bits:           %d\n", geometry_data->lbaf.lbk_len);

	printf("Media and Controller Capabilities:\n");
	printf("  Namespace supports Vector Chunk Copy:                 %s\n",
	       geometry_data->mccap.vec_chk_cpy ? "Supported" : "Not Supported");
	printf("  Namespace supports multiple resets a free chunk:      %s\n",
	       geometry_data->mccap.multi_reset ? "Supported" : "Not Supported");

	printf("Wear-level Index Delta Threshold:                       %d\n", geometry_data->wit);
	printf("Groups (channels):              %d\n", geometry_data->num_grp);
	printf("PUs (LUNs) per group:           %d\n", geometry_data->num_pu);
	printf("Chunks per LUN:                 %d\n", geometry_data->num_chk);
	printf("Logical blks per chunk:         %d\n", geometry_data->clba);
	printf("MIN write size:                 %d\n", geometry_data->ws_min);
	printf("OPT write size:                 %d\n", geometry_data->ws_opt);
	printf("Cache min write size:           %d\n", geometry_data->mw_cunits);
	printf("Max open chunks:                %d\n", geometry_data->maxoc);
	printf("Max open chunks per PU:         %d\n", geometry_data->maxocpu);
	printf("\n");
}
#elif defined(NVME_SSD_CONTROLLER)
void print_nvme_geometry(struct spdk_nvme_ctrlr *ctrlr) 
{
    printf("NVMe Device Geometry\n");
    printf("=======================\n");

    const struct spdk_nvme_ctrlr_data *cdata = spdk_nvme_ctrlr_get_data(ctrlr);

    printf("Model Number:                   %s\n", cdata->mn);
    printf("Serial Number:                  %s\n", cdata->sn);
    printf("Firmware Version:               %s\n", cdata->fr);
    printf("Max Data Transfer Size:         %d\n", cdata->mdts);

    int nsid;
    for (nsid = spdk_nvme_ctrlr_get_first_active_ns(ctrlr); nsid != 0; nsid = spdk_nvme_ctrlr_get_next_active_ns(ctrlr, nsid)) 
{
    struct spdk_nvme_ns *ns = spdk_nvme_ctrlr_get_ns(ctrlr, nsid);
    if (!ns) continue;

    const struct spdk_nvme_ns_data *nsdata = spdk_nvme_ns_get_data(ns);
    if (!nsdata) continue;

    printf("\nNamespace ID: %d\n", nsid);
    
    for (int lba_index = 0; lba_index < 16; lba_index++) 
    {
        if (!(nsdata->lbaf[lba_index].lbads)) continue;  // Check if this LBA format is supported by checking lbads (LBA Data Size)

        printf("LBA Format %d:\n", lba_index);
        printf("\tMetadata Size: %d bytes\n", nsdata->lbaf[lba_index].ms);
        printf("\tData Size: %d bytes\n", 1 << nsdata->lbaf[lba_index].lbads);
        printf("\tRelative Performance: %s\n", nsdata->lbaf[lba_index].rp == 0 ? "Best (0)" : (nsdata->lbaf[lba_index].rp == 1 ? "Better (1)" : (nsdata->lbaf[lba_index].rp == 2 ? "Good (2)" : "Degraded (3)")));
    }
}


    printf("\n");
}
#endif



#ifdef __OCSSD__CONTROLLER__
int nvme_ocssd_environment_init()
{
	char g_hostnqn[SPDK_NVMF_NQN_MAX_LEN + 1];
    uint32_t nsid;
	struct spdk_nvme_ctrlr_opts ctrlr_opts;

	/* initialize spdk_environment_options */
	spdk_env_opts_init(&opts);
    opts.name = "TiOCS";
    opts.core_mask = "0xF";
	opts.shm_id = 0;

    // /* initialize transport id */
	spdk_nvme_trid_populate_transport(&trid, SPDK_NVME_TRANSPORT_PCIE);
	snprintf(trid.subnqn, sizeof(trid.subnqn), "%s", SPDK_NVMF_DISCOVERY_NQN);
    strcpy(trid.traddr, "0000:00:05.0");

	if (trid.trtype != SPDK_NVME_TRANSPORT_PCIE) 
	{
		opts.no_pci = true;
	}
	if (spdk_env_init(&opts) < 0) 
	{
		printf("Failed to initialize VMD \nUnable to initialize SPDK env\n");
		return -1;
	}

	// assert(strlen(trid.traddr) != 0); 
	
	spdk_nvme_ctrlr_get_default_ctrlr_opts(&ctrlr_opts, sizeof(ctrlr_opts));
	memcpy(ctrlr_opts.hostnqn, g_hostnqn, sizeof(ctrlr_opts.hostnqn));
	ctrlr = spdk_nvme_connect(&trid, &ctrlr_opts, sizeof(ctrlr_opts));
	if(!ctrlr)
	{
		printf("Failed to connect to NVMe controller\n");
		return -1;
	}
    nsid = spdk_nvme_ctrlr_get_first_active_ns(ctrlr);
    ns = spdk_nvme_ctrlr_get_ns(ctrlr, nsid);

	/* initialize global variables for LOCS write/read/reset */
	page_size = spdk_nvme_ns_get_sector_size(ns);
    if(spdk_nvme_ctrlr_is_ocssd_supported(ctrlr))
    {
        spdk_nvme_ocssd_ctrlr_cmd_geometry(ctrlr, nsid, &geometry, sizeof(geometry),get_ocssd_geometry_completion,NULL);
    }
    else
    {
        printf("Not support OCSSD\n");
		return -1;
    }

	while (!geometry_completed)
    {
		spdk_nvme_ctrlr_process_admin_completions(ctrlr);
    }


	printf("---- SPDK Environment Initialized Successfully!\n");
	return 0;
}
#elif defined(NVME_SSD_CONTROLLER)
int variables_init()
{
    // chunk_capacity = (geometry.clba/64)*(( SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES* page_size) / sizeof(key_value_entry));
    // max_data_entries_per_io = SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES*page_size/sizeof(key_value_entry) ;
    fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m SSD Information Initialized Successfully!\n");
    return 0;
}

struct nvme_device_info* initialize_ns_info(struct spdk_nvme_ctrlr *ctrlr) 
{
    struct nvme_device_info *device_info = (struct nvme_device_info *)calloc(1, sizeof(struct nvme_device_info));
    if (!device_info) 
    {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Failed to allocate memory for device info.\n");
        return NULL;
    }

    device_info->ctrlr = ctrlr;
    device_info->num_ns = spdk_nvme_ctrlr_get_num_ns(ctrlr);
    
    // Allocate memory for the ns_info_array based on the number of namespaces
    device_info->ns_info_array = (struct nvme_ns_info *)calloc(device_info->num_ns, sizeof(struct nvme_ns_info));
    if (!device_info->ns_info_array) 
    {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Failed to allocate memory for namespaces info array.\n");
        free(device_info);
        return NULL;
    }

    for (uint32_t nsid = 1; nsid <= device_info->num_ns; nsid++) 
    {
        struct spdk_nvme_ns *ns = spdk_nvme_ctrlr_get_ns(ctrlr, nsid);
        if (!ns)
        {
            fprintf(stderr,"\033[0;31m[ERROR]\033[0m Failed to get namespace for NSID %d.\n", nsid);
            free(device_info->ns_info_array);
            free(device_info);
            return NULL;
        }
        int active_lba_format_index = spdk_nvme_ns_get_data(ns)->flbas.format & 0xf;  // Gets the current LBA format index.
        device_info->ns_info_array[nsid - 1].ns = ns;
        device_info->ns_info_array[nsid - 1].ns_id = nsid;
        device_info->ns_info_array[nsid - 1].sector_size = spdk_nvme_ns_get_sector_size(ns);
        device_info->ns_info_array[nsid - 1].lba_size = 1 << (spdk_nvme_ns_get_data(ns)->lbaf[active_lba_format_index].lbads); 
        device_info->ns_info_array[nsid - 1].ns_lba_count = spdk_nvme_ns_get_data(ns)->nsze;
        // printf("========== Namespace Info (ID: %u) ==========\n", nsid);
        // printf("%-20s: %p\n", "Namespace Pointer", device_info->ns_info_array[nsid - 1].ns);  // 假设ns是一个指针
        // printf("%-20s: %u\n", "Namespace ID", device_info->ns_info_array[nsid - 1].ns_id);
        // printf("%-20s: %u\n", "Sector Size", device_info->ns_info_array[nsid - 1].sector_size);
        // printf("%-20s: %u\n", "LBA Size", device_info->ns_info_array[nsid - 1].lba_size);
        // printf("%-20s: %u\n", "LBA Count", device_info->ns_info_array[nsid - 1].ns_lba_count);
        // printf("============================================\n");
    }
    return device_info;
}



int nvme_ssd_environment_init()
{
    char g_hostnqn[SPDK_NVMF_NQN_MAX_LEN + 1];
    uint32_t nsid;
    struct spdk_nvme_ctrlr_opts ctrlr_opts;

    /* Initialize spdk_environment_options */
    
    spdk_env_opts_init(&opts);
    opts.name = "No extra-IOs database";
    opts.core_mask = "0xF";
    opts.shm_id = 0;

    /* Initialize transport id */
    spdk_nvme_trid_populate_transport(&trid, SPDK_NVME_TRANSPORT_PCIE);
    snprintf(trid.subnqn, sizeof(trid.subnqn), "%s", SPDK_NVMF_DISCOVERY_NQN);
    strcpy(trid.traddr, "0000:81:00.0");

    if (trid.trtype != SPDK_NVME_TRANSPORT_PCIE) 
    {
        opts.no_pci = true;
    }
    if (spdk_env_init(&opts) < 0) 
    {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Failed to initialize VMD. Unable to initialize SPDK env.\n");
        return -1;
    }

    spdk_nvme_ctrlr_get_default_ctrlr_opts(&ctrlr_opts, sizeof(ctrlr_opts));
    memcpy(ctrlr_opts.hostnqn, g_hostnqn, sizeof(ctrlr_opts.hostnqn));
    ctrlr = spdk_nvme_connect(&trid, &ctrlr_opts, sizeof(ctrlr_opts));
    if(!ctrlr)
    {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Failed to connect to NVMe controller.\n");
        return -1;
    }

    print_nvme_geometry(ctrlr);
    nsid = spdk_nvme_ctrlr_get_first_active_ns(ctrlr);
    ns = spdk_nvme_ctrlr_get_ns(ctrlr, nsid);
    const struct spdk_nvme_ns_data *nsdata = spdk_nvme_ns_get_data(ns);

    // printf("The current active LBA format for namespace ID %d is: LBA Format %d  %lu\n", nsid, active_lba_format_index, nsdata->nsze);

    /* Initialize global variable for page size */
    page_size = spdk_nvme_ns_get_sector_size(ns);
    device_info = initialize_ns_info(ctrlr);
    if (!device_info) 
    {
        fprintf(stderr, "\033[0;31m[ERROR]\033[0m Failed to initialize namespace info.\n");
        return -1;
    }

    fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m SPDK Environment Initialized Successfully!\n");
    return 0;
}



void free_nvme_device_info() 
{
    if (device_info){
        // Free the array holding namespace information
        if (device_info->ns_info_array) {
            free(device_info->ns_info_array);
        }
        // Finally, free the device_info structure itself
        free(device_info);
    }
    fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m Device Info Freed Successfully!\n");
}


#endif