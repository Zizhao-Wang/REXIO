#include "syncstore.h"
#include "../../Backend/SSDWrite/writer.h"
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/IODisk/WriteDisk.h"

uint32_t SyncWrite(SKey key1, SValue value)
{

    /* Returned offset */
    uint32_t offset = 0;
    uint32_t curblock = 0;
    uint32_t curpage = 0;

	if(indexs >= 2048)
    {
        curblock = sectorpointer%4096>0?sectorpointer%4096<<21:0;
        curpage  = sectorpointer/4096 << 11; 
        SinglePageWrite();
        indexs = 0;
    }
	 
	Pagedata[indexs++] = key1;
	Pagedata[indexs++] = value;
	

    offset = indexs;
    curblock == 0 ? : offset & curblock;
    offset &= curpage;

    return offset;
}