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
int InfoRenew(size_t scale, PageType LogPointer=0, bool flag=false)
{

    if(flag)
    {
        DataPagePointer += (LogPointer/4096 == DataPagePointer/4096) ?scale:0;
        chunkusage[LogPointer/4096]= chunkusage[LogPointer/4096] + scale;
        //printf("LogPointer:%lu DataPagePointer:%lu chunkusage[LogPointer/4096]:%lu\n ",LogPointer,DataPagePointer,chunkusage[LogPointer/4096]);
    }
    else
    {
        chunkusage[DataPagePointer/4096]= chunkusage[DataPagePointer/4096] + scale;
        if(ChunkData[DataPagePointer/4096].size()>=820)
        {
            DataPagePointer += (4096-(DataPagePointer%4096));
        }
        else
        {
            DataPagePointer += scale;
        } 
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
        ChunkData[DataPagePointer/4096].emplace_back(DataPagePointer);
        InfoRenew(ws_min);   /* update pointers! */
    }
    else
    {
        EMessageOutput("Page writing failed in "+ Uint64toString(DataPagePointer)+"\n", 4598);
    }
    return err;
    
}

int PageLogWrite(uint64_t BlockId)
{

    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    PageType LogPagePointer = chunkusage[BlockId];
    //printf("LogPagePointer:%lu BlockID:%lu \n",LogPagePointer,BlockId);
    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, LogPagePointer+4096*BlockId);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];
    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
	    addrs[aidx].val = addrs_chunk.val;
	 	addrs[aidx].l.sectr =LogPagePointer+aidx;
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
        ChunkLog[BlockId].emplace_back(LogPagePointer) ;
        InfoRenew(ws_min,LogPagePointer+4096*BlockId,true);   /* update pointers! */
    }
    else
    {
        EMessageOutput("Page writing failed in "+ Uint64toString(LogPagePointer)+"\n", 4598);
    }
    return err;


}


/**
 * ============= Linear Hash module ===============
 *  Function declartion for writing data into one or more pages:
 **/
PageType SingleBucketWrite(std::vector<LHEntry> entries, uint64_t pageno)
{
    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    printf("Page number:%lu",pageno);
    if(pageno == UINT64_MAX)
    {
        pageno = sectorpointer;
        struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
        size_t ws_min = nvm_dev_get_ws_min(bp->dev);
        struct nvm_addr addrs[ws_min];
        for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
		    addrs[aidx].val = addrs_chunk.val;
		    addrs[aidx].l.sectr = (pageno%4096)+aidx;
	    }

        char * temp = new char[20];
        for (size_t i = 0; i < entries.size(); i++)
        {
            uint64_t *ML = (uint64_t*) temp;
            ML[0] = entries[i].key, ML[1] = entries[i].val;
            for(size_t j= i*sizeof(LHEntry)*8,k=0;j<i*sizeof(LHEntry)*8+sizeof(LHEntry)*8;j++,k++)
            {
                bp->bufs->write[j] = temp[k];
            }
        }

        // Write value into page. 
        err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
        if(err == 0) 
        {
            PointerRenew(ws_min);   /* update pointers! */
        }
    }

    return pageno;
    
}
