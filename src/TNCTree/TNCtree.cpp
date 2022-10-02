/**
 * @date    28/4/2022
 * @author  zz.wang
 * @details
 *          The front has a primary process used to reply to requests from users, including
 *          read, write and delete. In the meantime, frontend calls the backend process to read/write data
 *          from disk!
 * IDENTIFICATION:
 *             src/TNCTree/TNCtree.cpp
 **/

#include "TNCtree.h"
#include <iostream>
#include <vector>
#include "../Auxizilary/SysOutput.h"
#include "MemoryTier/MemTier.h"
#include "../Backend/SSDWrite/writer.h"
#include "../Backend/IODisk/WriteDisk.h"

void TNCtreeInit(void)
{

     int Createflag = ExtendHashTableInitialize();

     Pagedata = (uint64_t *)malloc(bp->geo->l.nbytes);
     if (Pagedata == nullptr)
     {
          EMessageOutput("Index initialized failure!",1578);
     }
        
     

     if(Createflag == 0)
     {
          printf("\n ================ Index information ================ \
            \n ---- Initialization successful!   \
            \n ---- 20 nodes tree has been initialized!\n");
     }

}

void TNCtreePort(void)
{
     
     TNCtreeInit();


     clock_t startTime,endTime;                        // Definition of timestamp

    /* Write datum */
    startTime = clock();
    for(uint64_t i=1;i<=1000000;i++)
    {
     InsertNode(i,i);
    }
    endTime = clock();
    std::cout << "Total Time of inserting: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


    /* Read datum */
    startTime = clock();
    for(int i=1;i<=1000000;i++)
    {
      uint64_t value = i;
      // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of reading data: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    /* data update */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
        uint64_t value = i;
        // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of datum update: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

}
