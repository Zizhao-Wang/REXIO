#include <spdk/nvme.h>
#include <spdk/env.h>
#include <spdk/log.h>
#include <spdk/nvme_ocssd.h>
#include <spdk/nvme_ocssd_spec.h>

static char g_hostnqn[SPDK_NVMF_NQN_MAX_LEN + 1];
static int g_controllers_found = 0;
struct spdk_nvme_transport_id trid; //transport id
static struct spdk_nvme_detach_ctx *g_detach_ctx = NULL;


static void
print_controller(struct spdk_nvme_ctrlr *ctrlr, const struct spdk_nvme_transport_id *trid,
		 const struct spdk_nvme_ctrlr_opts *opts)
{
	

	printf("=====================================================\n");
	
}



int main(int argc, char **argv)
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
		g_controllers_found++;
		print_controller(ctrlr, &trid, spdk_nvme_ctrlr_get_opts(ctrlr));
		spdk_nvme_detach_async(ctrlr, &g_detach_ctx);
	}

        return 0;
}