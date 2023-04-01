#include <ctime> //C system files
#include <iostream>//C++ system files 
#include <random> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <cstring>
#include <cstdlib>
#include <liblightnvm.h> // other project's .h files
#include <spdk/nvme.h> // SPDK's .h files
#include <spdk/env.h>
#include <spdk/log.h>
#include <spdk/nvme_ocssd.h>
#include <spdk/nvme_ocssd_spec.h>
#include "Ti-OCSSD/TiOCS.h"//your project's .h files
#include "LSMTree-NoFTL/StorageManager.h"
#include "MultiHash/ExtendibleHash/ExHash.h"
#include "MultiHash/LazySplitHash/LSHash.h"
#include "MultiHash/LinearHash/LiHash.h"
#include "Auxizilary/Logo.h"
#include "LSM-Tree/LsmTree.h"
#include "global_variables/global_variables.h"
#include "Backend/backend_variables.h"


/* Define some global variables. */
std::unordered_map<uint64_t,uint64_t> chunkusage;

static char g_hostnqn[SPDK_NVMF_NQN_MAX_LEN + 1];
static int g_controllers_found = 0;
struct spdk_nvme_transport_id trid; //transport id
static struct spdk_nvme_detach_ctx *g_detach_ctx = NULL;

int GlobalInitialize(int argc, char **argv)
{

	struct spdk_env_opts opts; //environment options
    
    struct spdk_nvme_ctrlr *ctrlr; // pointer that point to the controller
    const struct spdk_ocssd_geometry_data *geometry;
	bool g_vmd = false;

	spdk_env_opts_init(&opts);
    opts.name = "TiOCS";
    opts.core_mask = "0x1";
	opts.shm_id = 0;

	spdk_nvme_trid_populate_transport(&trid, SPDK_NVME_TRANSPORT_PCIE);


	snprintf(trid.subnqn, sizeof(trid.subnqn), "%s", SPDK_NVMF_DISCOVERY_NQN);

	if (trid.trtype != SPDK_NVME_TRANSPORT_PCIE) 
	{
		opts.no_pci = true;
	}

	if (spdk_env_init(&opts) < 0) 
	{
		fprintf(stderr, "Unable to initialize SPDK env\n");
		return 1;
	}

	

	
	strcpy(trid.traddr, "0000:00:05.0");
	printf("trtype: %u, traddr: %s, subnqn: %s\n", trid.trtype, trid.traddr, trid.subnqn);
	if(strlen(trid.traddr) != 0)
	{
		struct spdk_nvme_ctrlr_opts ctrlr_opts;
		spdk_nvme_ctrlr_get_default_ctrlr_opts(&ctrlr_opts, sizeof(ctrlr_opts));

		memcpy(ctrlr_opts.hostnqn, g_hostnqn, sizeof(ctrlr_opts.hostnqn));

		ctrlr = spdk_nvme_connect(&trid, &ctrlr_opts, sizeof(ctrlr_opts));

		if(!ctrlr)
		{
			printf("Failed to connect to NVMe controller\n");
		}
		printf("Successfully connected to NVMe controller\n");
		g_controllers_found++;
		spdk_nvme_detach_async(ctrlr, &g_detach_ctx);
	}


#ifdef USE_LIGHTNVM
    /* Initialize device information */
    bp = nvm_bp_init_from_args(argc,argv);
    if(!bp)
    {
        return -1;
    }

    /* Initialize global variables */

    for(size_t i=0;i<250;i++)
    {
        chunkusage[i] = 0;
    }
#endif

    // ws_min = nvm_dev_get_ws_min(bp->dev);
    // geo = nvm_dev_get_geo(bp->dev);

    // size_t chunk_width = geo->l.nsectr;
    // size_t lun_width = geo->l.npunit * chunk_width;
    // size_t channel_width = geo->l.npugrp * lun_width;
    // chunk_write_pointer = new size_t[geo->l.npugrp *geo->l.npunit*geo->l.nchunk];
    // chunk_write_pointer[0] = 0;
    // for (size_t i = 1; i < geo->l.npugrp *geo->l.npunit*geo->l.nchunk ; i++)
    // {
    //     chunk_write_pointer[i] = chunk_write_pointer[i-1]+chunk_width;
    // }

    // lun_current_pointer = new size_t[geo->l.npugrp * geo->l.npunit];
    // lun_current_pointer[0] = 0;
    // for(size_t i=1;i<geo->l.npugrp * geo->l.npunit;i++)
    // {
    //     lun_current_pointer[i] = lun_current_pointer[i-1] + lun_width;
    // }

    // channel_current_pointer = new size_t[geo->l.npugrp];
    // channel_current_pointer[0] = 0;
    // for(size_t i=1;i<geo->l.npugrp;i++)
    // {
    //     channel_current_pointer[i] = channel_current_pointer[i-1] + channel_width;
    // }

    // max_os_threads = 2;//std::thread::hardware_concurrency();

    // const char * process_Name = "Main of TiOCS";
    // prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(process_Name),0,0,0);

    printf(UCAS_SIAT);
    printf(Name);
    return 0;

    /* struct nvm_dev *dev = nvm_dev_open("/dev/nvme0n1");
    if (!dev) {
		perror("nvm_dev_open");
		return 1;
	} */

    /**
     * nvm_dev_pr(dev);
     * nvm_dev_close(dev);
     * 
    nvm_geo_pr(geo);
    struct nvm_async_ctx *ctx = nullptr;
	size_t depth=0;
    ctx = nvm_async_init(dev, depth,0);
	if (!ctx) {
		perror("could not initialize async context");
		return -1;
	}
    printf("nchunks:%lu tsectr:%lu \n ",bp->naddrs,bp->naddrs*bp->geo->l.nsectr);
    printf("geo nblocks:%lu nchannels:%lu nluns:%lu npages:%lu nplanes:%lu nsectors:%lu\n ",geo->nblocks,geo->nchannels,geo->nluns,geo->npages,geo->nplanes,geo->nsectors); */

}


int main(int argc, char **argv)
{

	/* initialize some global variables and SSD components initialization. */ 
    int result = GlobalInitialize(argc,argv);
    if(result != 0)
    {
        printf("Initialization failure!\n");
        exit(101);
    }

    /* select different indexes. line 49- */

    // EXHashing1();

    // TiOCSInit();

    // NoFTLKVInit();

    // LHashPort();

    // LSMTreePort();

    // LSHashPort();

    return 0;

    
        return 0;
}