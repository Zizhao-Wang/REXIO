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
#include "../../LSM-Tree/LsmTree.h"
#include "../../TNCTree/TNCtree.h"

std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkLog;
std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkData;
std::unordered_map<uint64_t,bool[1024]> GPT;
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
    readcount++;
    write++;
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
    write++;
    PageType LogPagePointer = chunkusage[BlockId];
    assert(LogPagePointer<=4092);
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
    for(int i=0;i<4096*4;i++)
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

int PageUpdate(PageType pageno, std::vector<LHEntry> entries)
{
   /* 
    * Step 1: Read all datum from original block. 
    */
    int err;
    uint64_t chunkno = pageno/4096;
    uint64_t updatesec = pageno%4096;
    uint64_t curseofchunk = chunkusage[chunkno];
    //printf("current sector pointer:%lu\n",curseofchunk);
    
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);

    // Read datum from original block/chunk.
    for (size_t sectr = 0; sectr < curseofchunk; sectr += ws_min) 
    {
        struct nvm_addr chunk_addrs[1];
        chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,sectr+chunkno*4096);
        size_t buf_ofz = sectr * bp->geo->l.nbytes;
		struct nvm_addr addrs[ws_min];
        //printf("==============\n");
		for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}
		err = nvm_cmd_read(bp->dev, addrs, ws_min, bp->bufs->read+buf_ofz, NULL, 0x0, NULL);

        char * temp = new char[20];
        for (size_t m = 0; m < CalculatePageCapacity(sizeof(LHEntry)); m++)
        {
            for(size_t k=0,j=buf_ofz+m*16;j<buf_ofz+m*16+16;j++,k++)
            {
                temp[k] = bp->bufs->read[j];
                bp->bufs->write[j] = temp[k];
            }
        }

		if (err == -1) 
        {
			printf("Read failure in part 1 of %ld page.\n",sectr);
			return -1;
		}
        //printf("Read part 1 succeed!\n");
    }
   /* 
    * Step 2: Erase original block. 
    */
    int eraseflag = erasechunk(pageno, chunkno);
    if(eraseflag == -1)
    {
        printf("Fatal error: chunk %lu erase failure.\n Error information: pageno:%lu chunkno:%lu",pageno/4096,pageno,chunkno);
    }

   /*
    * Step 3: Update datum that generate from step 1.
    */
    char * temp = new char[20];
    for (size_t i = 0; i < entries.size(); i++)
    {
        uint64_t *ML = (uint64_t*) temp;
        ML[0] = entries[i].key, ML[1] = entries[i].val;
        for(size_t j= (bp->geo->l.nbytes * updatesec)+i*sizeof(LHEntry),k=0;j<i*sizeof(LHEntry)+sizeof(LHEntry);j++,k++)
        {
            bp->bufs->write[j] = temp[k];
        }
    }

    /* Step 4: Write datum to the free-block. */
    for (size_t sectr = 0; sectr < curseofchunk; sectr += ws_min) 
    {
        //printf("Write start:\n");
        struct nvm_addr chunk_addrs[1];
        chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,sectr+chunkno*4096);
        size_t buf_ofz = sectr * bp->geo->l.nbytes;
		struct nvm_addr addrs[ws_min];
		for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}
		err = nvm_cmd_write(bp->dev, addrs, ws_min, bp->bufs->write+buf_ofz, NULL, 0x0, NULL);  
		if (err == -1) 
        {
			printf("Write failure in part 1 of %ld page.\n",sectr);
			return -1;
		}
        //printf("Re-write part 1 succeed!\n");
    }

    //printf("# Update completion in chunk: %ld\n", chunkno);
    return err;
}




PageType SingleBucketWrite(std::vector<LHEntry> entries, uint64_t pageno)
{
    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    
    if(pageno == UINT64_MAX)
    {
        writecount++;
        pageno = sectorpointer;
        //printf("pageno:%lu sectorpointer:%lu",pageno,sectorpointer);
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
            for(size_t j= i*sizeof(LHEntry),k=0;j<i*sizeof(LHEntry)+sizeof(LHEntry);j++,k++)
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
    else
    {
        writecount++;
        readcount++;
        erasecount++;
        int err = 0;
        err = PageUpdate(pageno, entries);
        if(err != 0)
        {
            EMessageOutput("PageUpdate failure in page: %lu"+Uint64toString(pageno),1500);
        }
    }
    //printf("Pageno %lu\n",pageno);
    return pageno;
    
}

/**
 * ============= LSM-tree module ===============
 *  Function declartion for writing data into one or more pages:
 **/

uint64_t SinglePageWrite(std::vector<entry_t> Entries, uint64_t pageno)
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

auto isEven = [](int x)->bool
{
    return x == false;

};

void GPTDisplay(void)
{
    for(int i=0;i<sectorpointer/4096;i++)
    {
        bool IsTrue = std::all_of(GPT[i],GPT[i]+1024,isEven);
        if(IsTrue)
        {
            printf("Block %d has been released!\n",i);
        }
    }
    printf("sectorpointer:%lu\n",sectorpointer);
}