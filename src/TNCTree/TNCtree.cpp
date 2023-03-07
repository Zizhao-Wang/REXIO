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
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>
#include "../Auxizilary/SysOutput.h"
#include "MemoryTier/MemTier.h"
#include "../Backend/SSDWrite/writer.h"
#include "../Backend/IODisk/WriteDisk.h"
#include "StoreTier/syncstore.h"

int reads = 0;
int writes = 0;
int erases = 0;


int sub_process_init(void)
{
     /*create a sub-process */
     pid_t pid;
     int status;
     key_t key = ftok("/home/TiOCS/src/shared_memory.txt", 'R');
     if (key == -1)
     {
          perror("Failed to create shared memory key.");
          return -1;
     }
     else
     {
          printf("Shared memory key created successfully.\n");
     }
     int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
     if (shmid == -1)
     {
          perror("Failed to create shared memory.");
          return -1;
     }
     int *data = (int*) shmat(shmid, NULL, 0); // 连接共享内存
     *data = 0; // inialize the shared memory

     if((pid = fork()) == -1 )
     {
          perror("fork error");
          return -1;
     }
     else if(pid == 0)
     {
          // child process
          //printf("child process
     }
     else
     {
          // parent process
          //printf("parent process
     }
     
     exit(0);
     
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

void TiOCSInit(void)
{
     
     if(sub_process_init() == 0)
     {
          printf("sub-process initialized failed!\n");
     }

     clock_t startTime,endTime;                        // Definition of timestamp

     /* workload a: insert only*/
     startTime = clock();
     for(SKey i=1;i<=40000000;i++)
     {
          if(i%10000000==0||i==1000000)
          {
               endTime = clock();
               std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
               printf("Read count:%d write:%d erase:%d\n",reads,writes,erases);
          }
          InsertNode(i,i);
     }
     endTime = clock();
     printf("Read count:%d write:%d erase:%d\n",reads,writes,erases);
     std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     uint64_t workb[1000010];
     string x;
     int index1=0;
     ifstream inFile;
     inFile.open("/home/femu/experiment1/code2/src/data/x2.txt");
     if (!inFile) 
     {
          std::cerr << "Unable to open file datafile.txt";
          exit(1);   // call system to stop
     }
     while (getline(inFile,x))     
     {
          workb[index1++] = atoi(x.c_str());
     }
     inFile.close();

     // for(int i=999991;i<1000000;i++)
     // {
     //      printf("Number:%lu\n",workb[i]);
     // }

     // uint64_t workc[1000010];
     // index1=0;
     // string y;
     // inFile.open("/home/femu/experiment1/code2/src/data/workloadC.txt");
     // if (!inFile) 
     // {
     //      std::cerr << "Unable to open file datafile.txt";
     //      exit(1);   // call system to stop
     // }
     // while (getline(inFile,y))     
     // {
     //      workc[index1++] = atoi(y.c_str());
     // }
     // inFile.close();
     // for(int i=1;i<10;i++)
     // {
     //      printf("Number:%lu\n",workc[i]);
     // }
     // exit(0);

     // for (size_t i = 32; i <= 1024; i=i*2)
     // {
          //printf("Buffer size:%lu\n",i);
          /* workload b: read only, all in it */

          // fifocache.Clear(i+2048);
          // // lrucache.ClearaReset(i+2048);
          // startTime = clock();
          // reads = 0;
          // write = 0;
          // erase = 0;

          // for(int i=1;i<=1000000;i++)
          // {
          //      Search(workb[i-1]);
          //      if(i%100000==0)
          //      {
          //           printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
          //           endTime = clock();
          //           std::cout << "Total Time of "<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
          //      }
          // }
          // endTime = clock();
          // printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
          // std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

          

     /* workload c: read only, 50% in it, 50% not in it */
     startTime = clock();
     reads = 0;
     writes = 0;
     erases = 0;
     for(int i=1;i<=1000000;i++)
     {
          Search(workb[i-1]);
          if(i%100000==0 || i==10000)
          {
               printf("Read count:%d write:%d erase:%d\n",reads,writes,erases);
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
          }
     }
     printf("Read count:%d write:%d erase:%d\n",reads,writes,erases);
     std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
          

     // /* workload d: update heavy workload, 50% read, 50% update */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%2==0)
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           Update(k,k+1);
     //      }
     //      if(i%100000==0 || i==10000)
     //      {
     //           printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      } 
     // }
     // printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
     // endTime = clock();
     // std::cout << "Total Time of workload d: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload E: read mostly workload, 95% read, 5% update */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%100<95)
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           Update(k,k+1);
     //      }
     //      if(i%100000==0 || i==10000)
     //      {
     //           printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      } 
     // }
     // printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
     // endTime = clock();
     // std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload F: read latest workload, 95% read, 5% insert */
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      if(i%100<95)
     //      {
     //           SKey k = 1+(rand()%40000000);
     //           Search(k);
     //      }
     //      else
     //      {
     //           SKey k = 40000000+(rand()%40000000);
     //           InsertNode(i+40000000,i+40000000);
     //      }
     //      if(i%100000==0 || i==10000)
     //      {
     //           printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      } 
     // }
     // printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
     // endTime = clock();
     // std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

     // /* workload G: delete workload, 100% delete*/
     // startTime = clock();
     // for(int i=1;i<=1000000;i++)
     // {
     //      srand48(time(NULL));
     //      SKey k = 1+(rand()%40000000);
     //      Delete(k);
     //      if(i%100000==0 || i==10000)
     //      {
     //           printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      }  
     // }
     // printf("Read count:%d write:%d erase:%d\n",reads,write,erase);
     // endTime = clock();
     // std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
     
}