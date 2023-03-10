#include <pthread.h>
#include <liblightnvm.h>
#include "parallel_writer.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../Auxizilary/pre_definition.h"
#include "../../global_variables/global_variables.h"



/*   */
int parallel_write_into_pu(char* buffer, PageType page_num)
{

    /* Function flag, default value equals 0(successful flag). */
    

    if(page_num == UINT64_MAX )
    {
        
        // struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, page_num);
        // size_t ws_min = nvm_dev_get_ws_min(bp->dev);
        // struct nvm_addr addrs[ws_min];
        // for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        // {
        //     addrs[aidx].val = addrs_chunk.val;
        //     addrs[aidx].l.sectr = (page_num%4096)+aidx;
        //     /* printf("aidx: %lu addrs: %lu")*/
        // }
    }

}


/**
 * ============= NoFTL-KV module ===============
 *  Function declartion for writing data into one or more pages:
 **/
uint64_t parallel_coordinator(std::vector<entry_t> run_data, uint64_t num_lun)
{

    /* create thread pool for asynchornous write */
    const nvm_geo *geo = nvm_dev_get_geo(bp->dev);
    pthread_t thread_id[geo->l.nchunk];

    /* write  */
    // size_t ws_min = nvm_dev_get_ws_min(bp->dev);

    // for (size_t i = 0; i < run_data.size(); i++)
    // {
    //     char thread_buffer[ws_min* bp->dev.geo.sector_nbytes];
        
    // }

    // if(pageno == UINT64_MAX )
    // {
        
    //     pageno = sectorpointer;
    //     struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
    //     
    //     struct nvm_addr addrs[ws_min];
    //     for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    //     {
	// 	    addrs[aidx].val = addrs_chunk.val;
	// 	    addrs[aidx].l.sectr = (pageno%4096)+aidx;
	// 	    /* printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr); */
	//     }

        
        
    //     // Write value into page. 
    //     err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
    //     if(err == 0) 
    //     {
    //         //printf("Insert completion! Insert sectors: %ld\n",sectorpointer);
    //         PointerRenew(ws_min);   /* update pointers! */
    //     }
    //     else
    //     {
    //         EMessageOutput("Page writing failed in "+ Uint64toString(pageno)+"\n", 4598);
    //     }
    // }

    return 0;

}