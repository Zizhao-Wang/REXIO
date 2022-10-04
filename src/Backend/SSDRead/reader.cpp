#include "reader.h"
#include "../IODisk/WriteDisk.h"

int SinglePageRead(uint64_t pageno)
{
    int err;
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

}