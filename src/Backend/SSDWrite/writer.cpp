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

std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkLog;
std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkData;
PageType DataPagePointer = 0;

/* function is used to update pointers. */
int InfoRenew(size_t scale)
{

    chunkusage[DataPagePointer/4096]= chunkusage[DataPagePointer/4096] + scale;
    DataPagePointer += scale;

    if(ChunkData[sectorpointer/4096].size()>=820 )
    {
        DataPagePointer += (4096-DataPagePointer%4096); 
    } 
    return 0;

}



/* 
 * Write functions. 
 */
int SinglePageWrite()
{

    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, DataPagePointer);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];
    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
	    addrs[aidx].val = addrs_chunk.val;
	 	addrs[aidx].l.sectr =(DataPagePointer%4096)+aidx;
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
        ChunkData[sectorpointer/4096].emplace_back(sectorpointer);
        InfoRenew(ws_min);   /* update pointers! */
    }
    else
    {
        EMessageOutput("Page writing failed in "+ Uint64toString(sectorpointer)+"\n", 4598);
    }
    return err;
    
}

int PageLogWrite(uint64_t BlockId)
{

    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    PageType LogPagePointer = chunkusage[BlockId];
    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, sectorpointer);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];
    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
	    addrs[aidx].val = addrs_chunk.val;
	 	addrs[aidx].l.sectr =(sectorpointer%4096)+aidx;
	}
        
    /* Write value into page. */ 
    char * databuffer = (char*) Pagedata;
    for(int i=0;i<2048*8;i++)
    {
        bp->bufs->write[i] = BufferLog[BlockId][i];
    }
    err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
    if(err == 0) 
    {
        ChunkLog[sectorpointer/4096].emplace_back(sectorpointer) ;
        PointerRenew(ws_min);   /* update pointers! */
    }
    else
    {
        EMessageOutput("Page writing failed in "+ Uint64toString(sectorpointer)+"\n", 4598);
    }
    return err;


}