#include <pthread.h>
#include <liblightnvm.h>
#include "parallel_writer.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../Auxizilary/pre_definition.h"
#include "../../global_variables/global_variables.h"
#include "../../Backend/backend_variables.h"
#include "../../Auxizilary/SysOutput.h"



/*   */
void* parallel_write_into_pu(void *args)
{

    struct thread_param *arg = (struct thread_param *)args;
    char *buffer = arg->buffer;
    uint64_t page_num = arg->page_num;
    int err = 0;

    if(page_num != UINT64_MAX )
    { 
        struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, page_num);
        struct nvm_addr addrs[ws_min];
        for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
            addrs[aidx].val = addrs_chunk.val;
            addrs[aidx].l.sectr = (page_num%4096)+aidx;
        }

        err = nvm_cmd_write(bp->dev, addrs, ws_min,buffer, NULL,0x0, NULL);
        if(err == 0) 
        {
            printf("Page %lu writing success \n",page_num);
        }
        else
        {
            EMessageOutput("Page writing failed in "+ Uint64toString(page_num)+"\n", 4598);
        }
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
    size_t pu_num = (num_lun-1) * (geo->l.nchunk*geo->l.nsectr);
    pthread_t thread_id[geo->l.nchunk];
    size_t page_size = ws_min * geo->sector_nbytes;
    char **buffer = new char*[geo->l.nchunk];


    for (size_t i = 0; i < geo->l.nchunk; i++)
    {
        buffer[i] = new char[page_size];
        for(size_t j= 0;j<page_size;j++)
        {
            buffer[i][j] = 'A';
        }
    }
    
    

    for (int i = 0; i < geo->l.nchunk; i++)
    {
        struct thread_param args;
        args.buffer = buffer[i];
        args.page_num = pu_num + i*geo->l.nsectr;
        pthread_create(&thread_id[i], NULL, parallel_write_into_pu, &(args));
    }
    

    return 0;

}