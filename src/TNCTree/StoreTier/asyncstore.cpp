#include "asyncstore.h"
#include <unistd.h>




int CreateMultiProcess()
{
}


// uint32_t AsyncWrite(SKey key, SValue value) 
// {   

//     if(indexs >= 2048)
//     {
//         WriteintoSector();
//         indexs = 0;
//     }


//     if(indexs <= 2047)
//     {
//         Pagedata [indexs++] = value;
//     }

//     /* Returned offset */
//     uint32_t offsetr = 0;

//     /*
//      * Block ID and page ID.
//      * Current offset of block and page.
//      */
//     curblock = 0x01;
//     uint_32 curpage = 0x01;


//     /*
//      * Maximum of page and block!
//      */
//     uint_32 bs = 256*1024;
//     uint_32 ps = 16*1024;

//     uint_32 size= sizeof(value);
//     blockoffset+=size;

//     if(blockoffset >= bs)
//     {
//         curblock++;
//         blockoffset=0x00;
//     }

//     return blockoffset;
// }