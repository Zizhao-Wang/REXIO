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
#include <liblightnvm.h> // other project's .h files
#include "TNCTree/TNCtree.h"//your project's .h files
#include "LSMTree-NoFTL/StorageManager.h"
#include "MultiHash/ExtendibleHash/ExHash.h"
#include "MultiHash/LazySplitHash/LSHash.h"
#include "MultiHash/LinearHash/LiHash.h"
#include "Auxizilary/Logo.h"
#include "LSM-Tree/LsmTree.h"

/* Define some global variables. */
struct nvm_bp* bp = nullptr;
std::unordered_map<uint64_t,uint64_t> chunkusage;

int GlobalInitialize(int argc, char **argv)
{

    /* Initialize device information */
    struct nvm_dev *dev = nvm_dev_open("/dev/nvme0n1");
    if (!dev) {
		perror("nvm_dev_open");
		return 1;
	}
    nvm_dev_pr(dev);
	nvm_dev_close(dev);
    const struct nvm_geo *geo = nvm_dev_get_geo(dev);
    
    printf("geo nblocks:%lu nchannels:%lu nluns:%lu npages:%lu nplanes:%lu nsectors:%lu\n ",geo->nblocks,geo->nchannels,geo->nluns,geo->npages,geo->nplanes,geo->nsectors);
    bp = nvm_bp_init_from_args(argc,argv);
    if(!bp)
    {
        return -1;
    }

    /* Initialize chunk information. */
    for(size_t i=0;i<250;i++)
    {
        chunkusage[i] = 0;
    }
    printf(UCAS_SIAT);
    printf(Name);
    return 0;
    
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

    // clock_t startTime,endTime;
    // startTime = clock();
    // std::set<int> a;
    // for(int i=0;i<=1000000;i++)
    // {
    //     a.insert(i);
    // }
    // endTime = clock();
    // std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    /* select different indexes. line 49- */

    // EXHashing1();

    // TNCtreePort();

    NoFTLKVInit();

    // LHashPort();

    // LSMTreePort();

    // LSHashPort();

    return 0;

}


