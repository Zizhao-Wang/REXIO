#include <pthread.h>
#include "parallelwriter.h"
#include "../../AuxizilaryFile/GlobalVariable.h"







/*   */
int parallel_write_into_pu(PageType page_num)
{

    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    if(page_num == UINT64_MAX )
    {
        LSMTreeWritePhysicalPage++;
        page_num = DataPagePointer;
        struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, page_num);
        size_t ws_min = nvm_dev_get_ws_min(bp->dev);
        struct nvm_addr addrs[ws_min];
        for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
            addrs[aidx].val = addrs_chunk.val;
            addrs[aidx].l.sectr = (page_num%4096)+aidx;
            /* printf("aidx: %lu addrs: %lu")*/
        }
    }

}


/**
 * ============= NoFTL-KV module ===============
 *  Function declartion for writing data into one or more pages:
 **/

uint64_t lun_parallel_write(uint64_t num_lun)
{

	

    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    if(pageno == UINT64_MAX )
    {
        LSMTreeWritePhysicalPage++;
        pageno = sectorpointer;
        struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
        size_t ws_min = nvm_dev_get_ws_min(bp->dev);
        struct nvm_addr addrs[ws_min];
        for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
		    addrs[aidx].val = addrs_chunk.val;
		    addrs[aidx].l.sectr = (pageno%4096)+aidx;
		    /* printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr); */
	    }

        char * temp = new char[20];
        for (size_t i = 0; i < Entries.size(); i++)
        {
            // printf("Value :%ld has been inserted!\n", ML[Cursize]);
            uint64_t *ML = (uint64_t*) temp;
            ML[0] = Entries[i].key, ML[1] = Entries[i].val;
            for(size_t j= i*sizeof(entry_t),k=0;j<i*sizeof(entry_t)+sizeof(entry_t);j++,k++)
            {
                bp->bufs->write[j] = temp[k];
            }
        }
        
        // Write value into page. 
        err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
        if(err == 0) 
        {
            //printf("Insert completion! Insert sectors: %ld\n",sectorpointer);
            PointerRenew(ws_min);   /* update pointers! */
        }
        else
        {
            EMessageOutput("Page writing failed in "+ Uint64toString(pageno)+"\n", 4598);
        }
    }

    return pageno;

}