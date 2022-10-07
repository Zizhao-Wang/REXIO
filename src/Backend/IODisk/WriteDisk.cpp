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
#include <unordered_map>
#include <stdlib.h>
#include <liblightnvm_cli.h>
#include <liblightnvm_spec.h>
#include "WriteDisk.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../BackendMain.h"

size_t writepointer = 0;
size_t blockpointer = 0;
size_t sectorpointer = 0;  /* sectorpointer means current write pointers. */
bool flag = false;




/* function is used to update pointers. */
int PointerRenew(size_t sectors)
{

    chunkusage[sectorpointer/4096]= chunkusage[sectorpointer/4096] + sectors;
    sectorpointer+=sectors; //update sector pointer.
 
    //printf("values after renewed: sector pointer: %lu,chunk pointer: %lu \n",sectorpointer,chunkusage[sectorpointer/4096]);
    return 0;

}


/* 
 * A compensate function is used to erase a specific chunk.
 * On success, 0 is returned. On error, -1 is returned.
 */
int CompenstaeFun(uint64_t chunkno)
{

    int err; // flag of nvm function.
    uint64_t secofchunk = chunkusage[chunkno];
    uint64_t sectors = chunkno*4096 + secofchunk;
    struct nvm_addr chunk_addrs[1];
    chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,sectors);
    size_t ws_opt = nvm_dev_get_ws_min(bp->dev);

    int sum=0;
    for (size_t sectr = secofchunk; sectr < bp->geo->l.nsectr; sectr += ws_opt) 
    {
        sum++;
        size_t buf_ofz = sectr * bp->geo->l.nbytes;
		struct nvm_addr addrs[ws_opt];
		for (size_t aidx = 0; aidx < ws_opt; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}
		err = nvm_cmd_write(bp->dev, addrs, ws_opt, bp->bufs->write+buf_ofz, NULL, 0x0, NULL);  
		if (err == -1) 
        {
			printf("Write failure in %ld sector of chunk %lu. Error information: secofchunk:%lu sectors:%lu\n",sectr,sectors/4096,secofchunk,sectors);
			return -1;
		}
    }
    //printf("After %d times compensation, compensation successful!\n",sum);
    
    return 0;
}



/*
 *  Erase a specific chunk/block. 
 */
int erasechunk(size_t pageno, uint64_t chunkno)
{

    uint64_t curseofchunk = chunkusage[chunkno];
    struct nvm_addr chunk_addrs[1];
    chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,pageno);

    if(curseofchunk < bp->geo->l.nsectr)
    {
        if(CompenstaeFun(chunkno) == -1)
        {
            printf("Compensation failure in chunk %lu!\n",chunkno);
            return -1;
        }
    }
    int err = nvm_cmd_erase(bp->dev, chunk_addrs, 1, NULL, 0x0, NULL);
    if(err == -1)
    {
        printf("chunk %lu erase failure.\n",chunkno);
    }
    return err;

}


int erasepage(size_t pageno, uint64_t chunkno)
{

    return 0;

}


/*
 *  this function is used to implement out-of-place update.
 */
int BlockUpdate(size_t pageno, uint64_t value, uint64_t Cursize)
{

   /* 
    * Step 1: Read all datum from original block. 
    */
    int err;
    uint64_t chunkno = pageno/4096;
    uint64_t updatesec = pageno%4096;
    uint64_t curseofchunk = chunkusage[chunkno];
    struct nvm_addr chunk_addrs[1];
    chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,pageno);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);

    // Read datum from original block/chunk.
    for (size_t sectr = 0; sectr < curseofchunk; sectr += ws_min) 
    {
        size_t buf_ofz = sectr * bp->geo->l.nbytes;
		struct nvm_addr addrs[ws_min];
		for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}
		err = nvm_cmd_read(bp->dev, addrs, ws_min, bp->bufs->read+buf_ofz, NULL, 0x0, NULL);
        for(size_t i=sectr*bp->geo->l.nbytes;i<ws_min * bp->geo->l.nbytes;i++)
        {
            bp->bufs->write[i] = bp->bufs->read[i];
        }  
		if (err == -1) 
        {
			printf("Read failure in part 1 of %ld page.\n",sectr);
			return -1;
		}
        printf("Read part 1 succeed!\n");
    }

   /* 
    * Step 2: Erase original block. 
    */
    int eraseflag = erasechunk(pageno, chunkno);
    if(eraseflag == -1)
    {
        printf("chunk %lu erase failure.\n",pageno/4096);
    }

   /*
    * Step 3: Update datum that generate from step 1.
    */
    char * temp = new char[100];
    int i=bp->geo->l.nbytes * updatesec;
    temp = &(bp->bufs->write[i]); 
    uint64_t *ML = (uint64_t*) temp;
    ML[Cursize] = value;

   /* 
    * Step 4: Find a free block.
    */
    unordered_map<uint64_t,uint64_t>::iterator it;
    for( it = chunkusage.begin();it!= chunkusage.end();it++) // for(auto& chunk : chunkusage)
    {
        if((*it).second == 0)
        {
            break;
        }
    }
    if((*it).first != chunkno)
    {
        chunkno = (*it).first;
        curseofchunk = chunkusage[chunkno];
        pageno = chunkno * (bp->geo->l.nsectr) + curseofchunk;
        chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,pageno);
    }

    /* Step 4: Write datum to the free-block. */
    for (size_t sectr = 0; sectr < curseofchunk; sectr += ws_min) 
    {
        printf("Write start:\n");
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
        printf("Re-write part 1 succeed!\n");
    }

   /* 
    * Step 5: update values in the in-memory hash table
    */

    printf("# Update completion in chunk: %ld\n", chunkno);
    return err;

}



/* 
 * Write functions. 
 */




uint64_t SSD_write2(uint64_t values)
{
   /*   uint64_t bpn;
    bpn = WriteintoSector();

    int err;
    //uint64_t chunkno = pageno / 4096;

    //某个块的 sector 开始位置 
    uint64_t sector =  chunkno * 4096;
    size_t ws_opt = nvm_dev_get_ws_min(bp->dev);

    // 判断当前块写到哪一个指针了 
    uint64_t sectorpointer = chunkusage[chunkno];

    // 2097152
    char * temp = new char[4097*4097];
    struct nvm_addr chunk_addrs[1];
    chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,sector);

    //读
    for(size_t sectr = 0; sectr < sectorpointer; sectr += ws_opt)
    {
        size_t buf_ofz = sectr * bp->geo->l.nbytes ;
        struct nvm_addr addrs[ws_opt];
        for (size_t aidx = 0; aidx < ws_opt; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}

        err = nvm_cmd_read(bp->dev, addrs, ws_opt,bp->bufs->read+buf_ofz, NULL,0x0, NULL);
    }
        
        // 写
    for(size_t i=0; i<sectorpointer*bp->geo->l.nbytes;i++)
    {
        temp[i] = bp->bufs->read[i]; 
    }
    uint64_t * ml = (uint64_t*) temp;

    for(uint64_t i = pageno*4096, j=0; i < pageno*4096+ 4096*4; i++)
    {
            ml[i] = values[j];
    }
    for(size_t i=0; i<sectorpointer*bp->geo->l.nbytes;i++)
    {
        bp->bufs->write[i] = temp[i];
    }
    
    // 擦除
    err = erasechunk(pageno);

    for(size_t sectr = 0; sectr < sectorpointer; sectr += ws_opt)
    {
        size_t buf_ofz = sectr * bp->geo->l.nbytes ;
        struct nvm_addr addrs[ws_opt];
        for (size_t aidx = 0; aidx < ws_opt; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}
        err = nvm_cmd_write(bp->dev, addrs, ws_opt,bp->bufs->write+buf_ofz, NULL,0x0, NULL);
    }
    return bpn;*/
}



int PageUpdate(size_t pageno, uint64_t value, uint64_t Cursize)
{
    /* 
    * Step 1: Read all datum from original block. 
    */
    int err;
    uint64_t chunkno = pageno/4096;
    uint64_t updatesec = pageno%4096;
    uint64_t curseofchunk = chunkusage[chunkno];
    struct nvm_addr chunk_addrs[1];
    chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,pageno);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);

    // Read datum from original block/chunk.
    for (size_t sectr = 0; sectr < curseofchunk; sectr += ws_min) 
    {
        size_t buf_ofz = sectr * bp->geo->l.nbytes;
		struct nvm_addr addrs[ws_min];
		for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}
		err = nvm_cmd_read(bp->dev, addrs, ws_min, bp->bufs->read+buf_ofz, NULL, 0x0, NULL);
        for(size_t i=sectr*bp->geo->l.nbytes;i<ws_min * bp->geo->l.nbytes;i++)
        {
            bp->bufs->write[i] = bp->bufs->read[i];
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
        printf("Fatal error: chunk %lu erase failure.\n Error information: pageno:%lu chunkno:%lu value:%lu Cursize:%lu ",pageno/4096,pageno,chunkno,value,Cursize);
    }

   /*
    * Step 3: Update datum that generate from step 1.
    */
    char * temp = new char[100];
    int i=bp->geo->l.nbytes * updatesec;
    temp = &(bp->bufs->write[i]); 
    uint64_t *ML = (uint64_t*) temp;
    ML[Cursize] = value;

    /* Step 4: Write datum to the free-block. */
    for (size_t sectr = 0; sectr < curseofchunk; sectr += ws_min) 
    {
        //printf("Write start:\n");
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

int PageUpdate(std::vector<uint64_t> pagelist, std::vector<entry_t> values)
{
    /* 
    * Step 1: Read all datum from original block. 
    */
    int err;
    

    uint64_t chunkno = pagelist[0]/4096;
    uint64_t updatesec = pagelist[0]%4096;
    uint64_t curseofchunk = chunkusage[chunkno];
    
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    uint64_t index = CalculatePageCapacity(sizeof(entry_t));

    // Read datum from original block/chunk.
    for (size_t sectr = 0; sectr < curseofchunk; sectr += ws_min) 
    {
        struct nvm_addr chunk_addrs[1];
        chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,pagelist[0]);

        size_t buf_ofz = sectr * bp->geo->l.nbytes;
		struct nvm_addr addrs[ws_min];
		for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}

		err = nvm_cmd_read(bp->dev, addrs, ws_min, bp->bufs->read+buf_ofz, NULL, 0x0, NULL);
        for(size_t i=sectr*bp->geo->l.nbytes;i<ws_min * bp->geo->l.nbytes;i++)
        {
            bp->bufs->write[i] = bp->bufs->read[i];
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
    int eraseflag = erasechunk(pagelist[0], chunkno);
    if(eraseflag == -1)
    {
        printf("Fatal error: chunk %lu erase failure.\n Error information: pageno:%lu chunkno:%lu ",pagelist[0]/4096,pagelist[0],chunkno);
    }

   /*
    * Step 3: Update datum that generate from step 1.
    */
    for(size_t i=0;i<pagelist.size();i++)
    {
        size_t buf_ofz = (i%4096) *bp->geo->l.nbytes;
        for (size_t j = 0; j < index; j++)
        {
            char * temp = new char[20];
            uint64_t *ML = (uint64_t*) temp;
            ML[0] = values[i*index+j].key, ML[1] = values[i*index+j].val; 
            for(size_t k=buf_ofz+j*sizeof(entry_t),m=0;k<buf_ofz+j*sizeof(entry_t)+sizeof(entry_t);k++,m++)
            {
                bp->bufs->write[k] = temp[m];
            }
        }
    }

    /* Step 4: Write datum to the free-block. */
    for (size_t sectr = 0; sectr < curseofchunk; sectr += ws_min) 
    {
        struct nvm_addr chunk_addrs[1];
        chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,pagelist[0]);
        //printf("Write start:\n");
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





/*
 * This function is used for single value insert. Thre are three parameters in this function:
 * @value:
 * @pageno: 
 * @Cursize: 
 */
uint64_t SingleValueWrite(uint64_t value, uint64_t pageno, uint64_t Cursize)
{

    /* Function flag, default value equals 0(successful flag). */
    int err = 0;

    /* Get chunkno, judge sector pointer of this block. */
    uint64_t  flag = pageno;
    if(pageno == UINT64_MAX)
    {
        pageno = sectorpointer;
    }

   /* 
    * Step 1 : 
    * Step 2 : 
    */
    if(flag == UINT64_MAX && Cursize == 0)
    {
        struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
        size_t ws_min = nvm_dev_get_ws_min(bp->dev);
        struct nvm_addr addrs[ws_min];
        for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
		    addrs[aidx].val = addrs_chunk.val;
		    addrs[aidx].l.sectr = (pageno%4096)+aidx;
		    /* printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr); */
	    }
        char * temp = new char[100];
        uint64_t *ML = (uint64_t*) temp;
        ML[Cursize] = value;
        //printf("Value :%ld has been inserted!\n", ML[Cursize]);
        for(int i=0;i<Cursize*8+10;i++)
        {
            bp->bufs->write[i] = temp[i]; 
        }
        // Write value into page. 
        err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
        if(err == 0) 
        {
            //printf("Insert completion! Insert sectors: %ld\n",sectorpointer);
            PointerRenew(ws_min);   /* update pointers! */
        }
    }
    else
    {
        PageUpdate(pageno,value,Cursize);
    }  

    return pageno;
}


int MultiPageWrite(std::vector<entry_t> entries, std::vector<uint64_t> pagelist)
{
    /* Function flag, default value equals 0(successful flag). */
    int err = 0;
    PageUpdate(pagelist,entries);
    return 0;
}



/*
 * Read functions. 
 */



int RunDataErase(std::vector<uint64_t> pageno)
{
    unordered_map<uint64_t,vector<uint64_t>> pagelist;
    for(size_t i=0;i<pageno.size();i++)
    {
        pagelist[pageno[i]/4096].emplace_back(pageno[i]);
    }
}



/**
 * Some other auxizilary functions.
 **/
uint64_t GetPagesize(void)
{
    return bp->geo->l.nbytes * nvm_dev_get_ws_min(bp->dev);
}

uint64_t CalculatePageCapacity(size_t KVsize)
{
    return GetPagesize()/KVsize;
}