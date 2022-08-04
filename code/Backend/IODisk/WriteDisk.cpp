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
#include "WriteDisk.h"
#include "../../AuxizilaryFile/GlobalVariable.h"
#include "../BackendMain.h"
#include <unordered_map>
#include <liblightnvm_cli.h>
#include <liblightnvm_spec.h>

uint_32 blockoffset   = 0;
size_t writepointer = 0;
size_t blockpointer = 0;
/* sectorpointer means current write pointers. */
size_t sectorpointer = 0;
int indexs=0;
bool flag = false;
uint64_t Pagedata[2050];


/* 
 * other compensate functions.
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
			printf("Write failure in %ld sector of chunk %lu.\n",sectr,sectors/4096);
			return -1;
		}
    }
    printf("After %d times compensation, compensation successful!\n",sum);
    
    return 0;

}



/*
 *  Erase functions. 
 */
int erasechunk(uint64_t sectorno)
{

    int err;
    uint64_t chunkno = sectorno/4096;
    struct nvm_addr chunk_addrs[1];
    chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,chunkno);
    if(chunkusage[sectorno/4096]!= 4092)
    {
        size_t ws_min = nvm_dev_get_ws_min(bp->dev);
        for (size_t sectr = chunkusage[sectorno/4096]; sectr < bp->geo->l.nsectr; sectr += ws_min) 
        {
			struct nvm_addr addrs[ws_opt];
			for (size_t aidx = 0; aidx < ws_opt; ++aidx) {
				addrs[aidx].val = chunk_addrs[0].val;
				addrs[aidx].l.sectr = sectr + aidx;
			}
			err = nvm_cmd_write(bp->dev, addrs, ws_opt, bp->bufs->write , NULL, 0x0, NULL);
			if (err) {
				perror("nvm_cmd_write");
				return -1;
			}
        }
    }
    printf("# chunk %ld will be erased!\n", chunkno);
    err = nvm_cmd_erase(bp->dev, chunk_addrs, 1, NULL, 0x0, NULL);
    return err;

}

/* On success, 0 is returned. On error, -1 is returned. */
int erasepage(uint64_t pageno)
{
    printf("After %d times read operation, Read part 2 succeed!\n",sum);

    /* Erase this chunck. */
    
    

    

    
    
    printf("# Erase completion in chunk: %ld\n", chunkno);
    return err;
    
}


/* function is used to update pointers. */
int PointerRenew(size_t sectors)
{

    sectorpointer+=sectors; //update sector pointer.

    chunkusage[sectorpointer/4096]= chunkusage[sectorpointer/4096] + sectors; //update chunk pointer.

    printf("values after renewed: sector pointer: %lu,chunk pointer: %lu \n",sectorpointer,chunkusage[sectorpointer/4096]);
    return 0;

}



int PageUpdate(size_t pageno)
{

    /* Step 1: Read all datum from original block. */

    int err;
    uint64_t chunkno = pageno%4096;
    uint64_t curseofchunk = chunkusage[pageno/4096];
    struct nvm_addr chunk_addrs[1];
    chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,pageno);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);

    // Read part 1 
    for (size_t sectr = 0; sectr < chunkno; sectr += ws_min) 
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

    int sum = 0;
    // Read part 2
    for (size_t sectr = chunkno+4; sectr < curseofchunk; sectr += ws_min) 
    {
        sum++;
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
        uint64_t *ML = (uint64_t*) bp->bufs->write;
        printf("values :%ld\n", ML[0]);

		if (err == -1) 
        {
			printf("Read failure in part 2 of %ld page.\n",sectr);
			return -1;
		}
    }

    /* Step 2: Erase original block. */
    
    if(curseofchunk < bp->geo->l.nsectr)
    {
        if(CompenstaeFun(chunkno) == -1)
        {
            printf("Compensation failure in chunk %lu!\n",chunkno);
            return -1;
        }
    }
    err = nvm_cmd_erase(bp->dev, chunk_addrs, 1, NULL, 0x0, NULL);
    if(err == -1)
    {
        printf("chunk %lu erase failure.\n",pageno/4096);
    }

    /* Step 2: update datum that generate from step 1. */

    /* Step 3: find a free block */

    /* Step 4: Write datum to another free-block. */
    // Write part 1.
    for (size_t sectr = 0; sectr < chunkno; sectr += ws_min) 
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

    // Re-write part 2. 
    for (size_t sectr = chunkno; sectr < curseofchunk; sectr += ws_min) 
    {
        printf("Re-write start: \n");
        size_t buf_ofz = sectr * bp->geo->l.nbytes;
		struct nvm_addr addrs[ws_min];
		for (size_t aidx = 0; aidx < ws_min; ++aidx) 
        {
			addrs[aidx].val = chunk_addrs[0].val;
			addrs[aidx].l.sectr = sectr + aidx;
		}
        printf("Re-write start:\n");
        if(sectr != 0)
		err = nvm_cmd_write(bp->dev, addrs, ws_min, bp->bufs->write, NULL, 0x0, NULL);
		if (err == -1) 
        {
			printf("Write failure in part 2 of %ld page.\n",sectr);
			return -1;
		}
        printf("Re-write part 2 succeed!\n");
    }


    /* Step 5: update values in the in-memory hash table*/


}



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

    char * databuffer = (char*)Pagedata;
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


uint_32 SSD_write(uint64_t value) 
{
    int err = EXIT_FAILURE;   

    if(indexs >= 2048)
    {
        WriteintoSector();
        indexs = 0;
    }


    if(indexs <= 2047)
    {
        Pagedata [indexs++] = value;
    }

    /*
     * Return offset;
     */
    uint_32 ID = 0;

    /*
     * Block ID and page ID.
     * Current offset of block and page.
     */
    uint_32 curblock = 0x01;
    uint_32 curpage = 0x01;


    /*
     * Maximum of page and block!
     */
    uint_32 bs = blocksize*1024;
    uint_32 ps = pagesize*1024;

    uint_32 size= sizeof(value);
    blockoffset+=size;

    if(blockoffset >= bs)
    {
        curblock++;
        blockoffset=0x00;
    }

    return blockoffset;
}


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


/*
 * This function is used for single value insert. 
 */
uint64_t SVwrite(uint64_t value, uint64_t pageno, uint64_t Cursize)
{

    /* Function flag. */
    int err;

    /* Get chunkno, judge sector pointer of this block. */
    uint64_t  flag = pageno;
    if(pageno == UINT64_MAX)
    {
        pageno = sectorpointer;
    }

    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];
    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
		addrs[aidx].val = addrs_chunk.val;
		addrs[aidx].l.sectr = (pageno%4096)+aidx;
		/* printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr); */
	}

    if(flag != UINT64_MAX)
    {
        /* Read datum from page. */
        // printf("pageno: %lu value: %lu \n",pageno,value);
        err = nvm_cmd_read(bp->dev, addrs, ws_min,bp->bufs->read, NULL,0x0, NULL);
        if(err == -1)
        {
            printf("NVM read failure in %ld page.\n",pageno);
        }
        /* Update values in buffer.  */
        uint64_t *Read2Sectors = (uint64_t *) bp->bufs->read;
        Read2Sectors[Cursize] = value;
        for(int i=0;i<Cursize*8+10;i++)
        {
            bp->bufs->write[i] = bp->bufs->read[i];
        }
        // printf("%lu %lu\n",Read2Sectors[0],Read2Sectors[1]);
    }
    else
    {
        char * temp = new char[100];
        for(int i=0;i<Cursize*8+10;i++)
        {
            temp[i] = bp->bufs->write[i]; 
        }
        uint64_t *ML = (uint64_t*) temp;
        ML[Cursize] = value;
        printf("values :%ld\n", ML[0]);
        for(int i=0;i<Cursize*8+10;i++)
        {
            bp->bufs->write[i] = temp[i]; 
        }
        for(int i=0;i<Cursize*8+10;i++)
        {
            temp[i] = bp->bufs->write[i]; 
        }
        printf("values :%ld\n", ML[0]);
    }

    /* Erase the block. */
    
    if(flag != UINT64_MAX)
    {
        printf("Page %lu need to modify.\n",pageno);
        erasepage(pageno);
    }
        

    /* Write value into page. */
    if(value == 2)
    err = nvm_cmd_write(bp->dev, addrs, ws_min,bp->bufs->write, NULL,0x0, NULL);
 
    if(1)
    {
        
        printf("Insert completion! Insert sectors: %ld\n",sectorpointer);
        /* update pointers! */
        PointerRenew(ws_min);
    }
    return pageno;

}



/*
 * Read functions. 
 */
int ReadfromPage(uint64_t pageno)
{

    int err;
    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];
    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
		addrs[aidx].val = addrs_chunk.val;
		addrs[aidx].l.sectr = pageno%4096+aidx;
		/* printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr); */
	}
    //err = nvm_cmd_read(bp->dev, addrs, ws_min,bp->bufs->read, NULL,0x0, NULL);
    if(err == -1)
    {
        printf("Reading page %ld failure.\n",pageno);
    }
    
    return 0;

}
