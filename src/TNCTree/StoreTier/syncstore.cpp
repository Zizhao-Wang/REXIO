#include "syncstore.h"
#include "../../Backend/SSDWrite/writer.h"
#include "../../Backend/SSDRead/reader.h"
#include "../../Backend/IODisk/WriteDisk.h"

uint32_t SyncWrite(SKey key1, SValue value)
{

    /* Returned offset */
    uint32_t offset = 0;


	if(indexs >= CalculatePageCapacity(sizeof(TNCEntry)))
    {

        SinglePageWrite();
        offset += sectorpointer%4096==0 && sectorpointer!=0?0x400000:0x00000000 ;
        offset += 0x1000; 
        indexs = 0;
    }
	 
	Pagedata[indexs].key = key1;
	Pagedata[indexs].val = value;
	++indexs;

    ++offset;

    return offset;
}