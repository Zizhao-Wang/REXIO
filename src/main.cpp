/**
 * @date:   8/4/2022
 * @author: zz.wang
 * @email:  zz.wang@siat.ac.cn
 * @details:
 *          This file is the unique port of this project.
 * IDENTIFICATION:
 *          src/main.cpp
 *
 * According to the Google CPP style guide, header file reference should follow the following order:
 * (To avoid hidden dependencies)
 * 1. directly related header 2. C library 3. C++ library,
 * 4. other libraries'.h  5. your project's .h.  
 **/

#include <ctime> //C system files
#include <iostream>//C++ system files 
#include <random> 
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <cstring>
#include <cstdlib>
#include <liblightnvm.h> // other project's .h files
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


int GlobalInitialize(int argc, char **argv)
{

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


    ws_min = nvm_dev_get_ws_min(bp->dev);
    geo = nvm_dev_get_geo(bp->dev);

    size_t chunk_width = geo->l.nsectr;
    size_t lun_width = geo->l.npunit * chunk_width;
    size_t channel_width = geo->l.npugrp * lun_width;
    chunk_write_pointer = new size_t[geo->l.npugrp *geo->l.npunit*geo->l.nchunk];
    chunk_write_pointer[0] = 0;
    for (size_t i = 1; i < geo->l.npugrp *geo->l.npunit*geo->l.nchunk ; i++)
    {
        chunk_write_pointer[i] = chunk_write_pointer[i-1]+chunk_width;
    }

    lun_current_pointer = new size_t[geo->l.npugrp * geo->l.npunit];
    lun_current_pointer[0] = 0;
    for(size_t i=1;i<geo->l.npugrp * geo->l.npunit;i++)
    {
        lun_current_pointer[i] = lun_current_pointer[i-1] + lun_width;
    }

    channel_current_pointer = new size_t[geo->l.npugrp];
    channel_current_pointer[0] = 0;
    for(size_t i=1;i<geo->l.npugrp;i++)
    {
        channel_current_pointer[i] = channel_current_pointer[i-1] + channel_width;
    }

    max_os_threads = 2;//std::thread::hardware_concurrency();

    const char * process_Name = "Main of TiOCS";
    prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(process_Name),0,0,0);

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

    NoFTLKVInit();

    // LHashPort();

    // LSMTreePort();

    // LSHashPort();

    return 0;

}


