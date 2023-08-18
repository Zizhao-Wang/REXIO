#include "global_variables.h"
#include <vector>
#include "../Auxizilary/pre_definition.h"


/*Definition of global variables. */
struct spdk_ocssd_geometry_data geometry;

int backend_commands = 0;

struct spdk_env_opts opts; //environment options
    
struct spdk_nvme_ctrlr *ctrlr = nullptr; // pointer that point to the controller

struct spdk_nvme_transport_id trid;

struct spdk_nvme_ns * ns = nullptr;

#ifdef MULTI_THREAD_IO
std::map<pthread_t,uint64_t> temp_pointers;	
#elif defined(SINGLE_THREAD_IO)
uint64_t temp_pointers;
#endif

struct spdk_nvme_detach_ctx *g_detach_ctx = NULL;

uint64_t chunk_capacity;

uint64_t max_data_entries_per_io;

uint64_t page_size;

bool geometry_completed = false;


uint64_t get_max_data_entries_per_io()
{
	return (page_size* SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES)/sizeof(entry_t);
}


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

 int variables_init()
 {
	chunk_capacity = (geometry.clba/64)*(( SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES* page_size) / sizeof(entry_t));
	max_data_entries_per_io = SPDK_NVME_OCSSD_MAX_LBAL_ENTRIES*page_size/sizeof(entry_t) ;
	printf("---- SSD Information Initialized Successfully! \n");
	return 0;
 }


void get_ocssd_geometry_completion(void *cb_arg, const struct spdk_nvme_cpl *cpl)
{
	if (spdk_nvme_cpl_is_error(cpl)) 
    {
		printf("get ocssd geometry failed\n");
	}
    else
    {
		variables_init();
        backend_commands--;
		geometry_completed = true;
    }
}



int environment_init()
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