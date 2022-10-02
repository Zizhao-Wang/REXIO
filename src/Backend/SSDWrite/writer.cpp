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
uint64_t * Pagedata = (uint64_t *)malloc(bp->geo->l.nbytes);



/* 
 * Write functions. 
 */
int WriteintoSector()
{
    uint64_t bpn = 0;
    printf("Starting writing to SSD:\n");
    if(!bp)
    {
        perror("nvm_bp_init failure!");
        exit(101);
    }

    int err;

    //Initialize sector address.
    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev,sectorpointer);

    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];

    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
		addrs[aidx].val = addrs_chunk.val;
		addrs[aidx].l.sectr = writepointer+aidx;
		//printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr);
	}

	char * databuffer = (char*) Pagedata;
    for(int i=0;i<2048*8;i++)
        bp->bufs->write[i] = databuffer[i];
    err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
    if(!err)
    {
        printf("Insert completion! Insert sectors: %ld\n",sectorpointer);
    }

    bpn = sectorpointer;
    //update write pointer!
    sectorpointer+= ws_min;
    writepointer += ws_min;
    //update data structure!
    chunkusage[sectorpointer/4096] = sectorpointer%4096;
    

             
    if (writepointer>=4096)
    {
        blockpointer++;
        writepointer = 0;
    }
    
    return bpn;
}


/**
 *  
 **/
int SinglePageWrite()
{

    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    // struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
    // size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    // struct nvm_addr addrs[ws_min];
    // for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    // {
	// 	addrs[aidx].val = addrs_chunk.val;
	// 	addrs[aidx].l.sectr = (pageno%4096)+aidx;
	// 	/* printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr); */
	// }

    // char * temp = new char[20];
    // for (size_t i = 0; i < Entries.size(); i++)
    // {
    //     // printf("Value :%ld has been inserted!\n", ML[Cursize]);
    //     uint64_t *ML = (uint64_t*) temp;
    //     ML[0] = Entries[i].key, ML[1] = Entries[i].val;
    //     for(size_t j= i*sizeof(entry_t)*8,k=0;j<i*sizeof(entry_t)*8+sizeof(entry_t)*8;j++,k++)
    //     {
    //             bp->bufs->write[i] = temp[i];
    //     }
	// }
        
    // // Write value into page. 
    // err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
    // if(err == 0) 
    // {
    //     //printf("Insert completion! Insert sectors: %ld\n",sectorpointer);
    //     PointerRenew(ws_min);   /* update pointers! */
    // }
    // else
    // {
    //         EMessageOutput("Page writing failed in "+ Uint64toString(pageno)+"\n", 4598);
    // }

    return err;

}
