#include "reader.h"
#include "../IODisk/WriteDisk.h"
#include "../../MultiHash/LinearHash/LiHash.h"
#include "../../LSM-Tree/LsmTree.h"

int SinglePageRead(uint64_t pageno)
{
    readcount++;
    int err;
    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];

    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
		addrs[aidx].val = addrs_chunk.val;
		addrs[aidx].l.sectr = (pageno % bp->geo->l.nsectr) + aidx;
	}
    
    err = nvm_cmd_read(bp->dev, addrs, ws_min,bp->bufs->read, NULL,0x0, NULL);
    if(err == -1)
    {
        printf("Reading page %ld failure.\n",pageno);
        EMessageOutput("Run data read failure in page"+ Uint64toString(pageno)+"\n",106);
        return -1;
    }
    
    return 0;

}


TNCEntry* TNCEntryRead(PageType PageId)
{
    
	size_t capacity = CalculatePageCapacity(sizeof(TNCEntry));
	TNCEntry* data = new TNCEntry[capacity+10];
	size_t pos = 0;

	SinglePageRead(PageId);

	char * temp = new char[sizeof(TNCEntry)];
    TNCEntry TempEntry;
    for(size_t i = 0;i<capacity;i++)
	{
		for(size_t j = i*sizeof(entry_t),k=0;j<i*sizeof(TNCEntry)+sizeof(TNCEntry);j++,k++)
    	{
         	temp[k] = bp->bufs->read[j];
    	}
		uint64_t *ML = (uint64_t*) temp;
		TempEntry.key = ML[0], TempEntry.val = ML[1];
		data[pos++] = TempEntry;
	}
	delete(temp);
    
	return data;

}


/** 
 * ================= Linear Hash module ==================== 
 *   Read functions for Linear Hashing! 
 **/
std::vector<LHEntry> PageRead(PageType PageNum)
{
	std::vector<LHEntry> entries;

    // clock_t startTime,endTime;  // Definition of timestamp
    // startTime = clock();
	SinglePageRead(PageNum);
    // endTime = clock();
    // std::cout << "Total Time : " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    char * temp = new char[20];
    LHEntry entry;
    for (size_t i = 0; i < CalculatePageCapacity(sizeof(LHEntry)); i++)
    {
        for(size_t j = i*sizeof(LHEntry),k=0;j<i*sizeof(LHEntry)+sizeof(LHEntry);j++,k++)
        {
            temp[k] = bp->bufs->read[j];
        }
        uint64_t *ML = (uint64_t*) temp;
        entry.key = ML[0], entry.val = ML[1];
        entries.emplace_back(entry);
    }  

    return entries;
	
}

/**
 * ============= Linear Hash module ===============
 *  Function declartion for writing data into one or more pages:
 **/
int PageDataRead(PageType pageno)
{
    int err;
    LSMTreeReadPhysicalPage++;
    struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, pageno);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];

    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
		addrs[aidx].val = addrs_chunk.val;
		addrs[aidx].l.sectr = pageno % bp->geo->l.nsectr + aidx;
	}
    err = nvm_cmd_read(bp->dev, addrs, ws_min,bp->bufs->read, NULL,0x0, NULL);
    if(err == -1)
    {
        printf("Reading page %ld failure.\n",pageno);
        EMessageOutput("Run data read failure in page"+ Uint64toString(pageno)+"\n",106);
        return -1;
    }
    
    return 0;
}


std::vector<entry_t> RunReadFromPage(PageType PageNum)
{

    std::vector<entry_t> data;
    assert(PageNum != UINT64_MAX);
    PageDataRead(PageNum);
    
    char * temp = new char[20];
    entry_t TempEntry;
    for (size_t i = 0; i <CalculatePageCapacity(sizeof(entry_t)); i++)
    {
        // printf("Value :%ld has been inserted!\n", ML[Cursize]);
        for(size_t j = i*sizeof(entry_t),k=0;j<i*sizeof(entry_t)+sizeof(entry_t);j++,k++)
        {
            temp[k] = bp->bufs->read[j];
        }
        uint64_t *ML = (uint64_t*) temp;
        TempEntry.key = ML[0], TempEntry.val = ML[1];
        data.emplace_back(TempEntry);
    }
    // for (size_t i = 0; i < 5; i++)
    // {
    //     printf("%lu %lu ",data[i].key,data[i].val);
    // }
    // printf("\n");
    //printf("%lu data entries have beed read!\n",data.size());

    delete(temp);
    return data;

}