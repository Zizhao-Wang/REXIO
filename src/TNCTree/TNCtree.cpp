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
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include "../Auxizilary/SysOutput.h"
#include "MemoryTier/MemTier.h"
#include "../Backend/SSDWrite/writer.h"
#include "../Backend/IODisk/WriteDisk.h"
#include "StoreTier/syncstore.h"

void TNCtreeInit(void)
{

     int Createflag = ExtendHashTableInitialize();

     // Pagedata = (TNCEntry *)malloc(bp->geo->l.nbytes);
     // if (Pagedata == nullptr)
     // {
     //      EMessageOutput("Index initialized failure!",1578);
     // }

     if(Createflag == 0)
     {
          printf("\n ================ Index information ================ \
            \n ---- Initialization successful! \
            \n ---- Meta data space allocated successful! \n");
     }

}

void TNCtreePort(void)
{
     
     TNCtreeInit();

     clock_t startTime,endTime;                        // Definition of timestamp

     /* workload a: insert only*/
     startTime = clock();
     for(SKey i=1;i<=50000000;i++)
     {
          // if(i!=0&&i%100000==0)
          // printf("Insert %lu successful!\n",i);
          InsertNode(i,i);
     }
     endTime = clock();
     std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


     /* workload b: read only, all in it */
     startTime = clock();
     for(int i=1;i<=1000000;i++)
     {
          srand48(time(NULL));
          SKey k = 1+(rand()%50000000);
          Search(k);
          if(i==10000 || i%100000==0)
          {
               endTime = clock();
               std::cout << "Total Time of"<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
          }
     }
     endTime = clock();
     std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     /* workload c: read only, 50% in it, 50% not in it */
     startTime = clock();
     
     for(int i=1;i<=1000000;i++)
     {
          srand48(time(NULL));
          if(i%100<50)
          {
               SKey k = 1+(rand()%60000000);
               Search(k);
          }
          else
          {
               SKey k = 50000000+(rand()%60000000);
               Search(k);
          }
          if(i%100000==0 || i==10000)
          {
               endTime = clock();
               std::cout << "Total Time of"<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
          }
     }
     endTime = clock();
     std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     /* workload d: update heavy workload, 50% read, 50% update */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%2==0)
     //      {
     //           SKey k = 1+(rand()%60000000);
     //           Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%60000000);
     //           Update(k,k+1);
     //      } 
     // }
     // endTime = clock();
     // std::cout << "Total Time of workload d: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload E: read mostly workload, 95% read, 5% update */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%100<95)
     //      {
     //           SKey k = 1+(rand()%60000000);
     //           Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%60000000);
     //           Update(k,k+1);
     //      } 
     // }
     // endTime = clock();
     // std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload F: read latest workload, 95% read, 5% insert */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%100<95)
     //      {
     //           SKey k = 1+(rand()%50000000);
     //           Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%50000000);
     //           InsertNode(i+50000000,i+50000000);
     //      } 
     // }
     // endTime = clock();
     // std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload G: delete workload, 100% delete*/
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      SKey k = 1+(rand()%50000000);
     //      Delete(k); 
     // }
     // endTime = clock();
     // std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
     
}
