/**
 * @date 2022.5.15 11:22
 * @author zz.wang
 * @details
 *          There are two main processes in the backend. More details about them are as follows::
 *
 *           Backend write process:
 *           Backend read process:
 *
 *         IDENTIFICATION
 *   Experiment1/Backend/BackendMain.h
 */

#include "writer.h"

#include <liblightnvm_cli.h>
#include <liblightnvm_spec.h>
#include "../IODisk/WriteDisk.h"
#include "../../Auxizilary/GlobalVariable.h"


uint_32 blockoffset = 0;
int indexs=0;
TNCEntry * Pagedata = nullptr;


/* function is used to update pointers. */
int InfoRenew(size_t scale)
{

    sectorpointer+=scale; //update sector pointer.
    chunkusage[sectorpointer/4096]= chunkusage[sectorpointer/4096] + scale;
    
    if(sectorpointer%4096 == 3536)
    {
        sectorpointer += 140*4; 
    } 
    //printf("values after renewed: sector pointer: %lu,chunk pointer: %lu \n",sectorpointer,chunkusage[sectorpointer/4096]);
    return 0;
}





/* 
 * Write functions. 
 */
int SinglePageWrite()
{

    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, sectorpointer);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];
    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
	    addrs[aidx].val = addrs_chunk.val;
	 	addrs[aidx].l.sectr =(sectorpointer%4096)+aidx;
	 	/* printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr); */
	}
        
    /* Write value into page. */ 
    char * databuffer = (char*) Pagedata;
    for(int i=0;i<2048*8;i++)
    {
        bp->bufs->write[i] = databuffer[i];
    }
    err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
    if(err == 0) 
    {
        //printf("Insert completion! Insert sectors: %ld\n",sectorpointer);
        InfoRenew(ws_min);   /* update pointers! */
    }
    else
    {
        EMessageOutput("Page writing failed in "+ Uint64toString(sectorpointer)+"\n", 4598);
    }

    return err;

}
