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

#include "TiOCS.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include "../Auxizilary/SysOutput.h"
#include "../LOCS/global_variables.h"
#include "StoreTier/syncstore.h"
#include "MemoryTier/MemTier.h"
#include "../Backend/SSDWrite/writer.h"
#include "../Backend/IODisk/WriteDisk.h"
#include "StoreTier/syncstore.h"
#include "../Backend/backend_variables.h"
#include "StoreTier/io_manager.h"

#define MAX_MSG_SIZE 1024

volatile int msgid;

struct mymsgbuf 
{  
     long mtype;  
     char mtext[MAX_MSG_SIZE];  
};  



int print_init_info(void)
{
     int Createflag = ExtendHashTableInitialize();
     if(Createflag != 0)
     {
          return -1;
     }

     sectors_per_page = geometry.ws_min;
     buffer_capacity = sectors_per_page * geometry.clba / (KEY_SIZE+VAL_SIZE);

     for (uint64_t i = 0; i < geometry.num_grp*geometry.num_pu*geometry.num_chk; ++i) 
     {
          BlockWritePointers[i] = 0;
          block_information[i] = make_pair(0,0);
     }

     buffer_init();
     kv_buffer_init();


     size_t kv_pairs_per_page = sectors_per_page*geometry.clba/(KEY_SIZE+VAL_SIZE);
     size_t logs_per_page = sectors_per_page*geometry.clba/(sizeof(uint32_t));
     size_t pages_per_block = geometry.clba/sectors_per_page;
     num_data_page = pages_per_block - (pages_per_block)/(logs_per_page/kv_pairs_per_page +1);

     size_t keys_per_page = sectors_per_page*geometry.clba/(KEY_SIZE);
     key_num_data_page = num_data_page*kv_pairs_per_page;

     size_t values_per_page = sectors_per_page*geometry.clba/(VAL_SIZE);
     value_num_data_page = num_data_page*kv_pairs_per_page;

     if(ctrlr == NULL)
	{
		printf("ctrlr is NULL!\n");
		return -1;
	}
     qpair = spdk_nvme_ctrlr_alloc_io_qpair(ctrlr, NULL, 0);
     if(qpair == NULL)
     {
          printf("ERROR: spdk_nvme_ctrlr_alloc_io_qpair() failed\n");
          return -1;
     }

	
     printf("\n ================ Index information ================ \
            \n ---- child process created successful! \
            \n ---- In-memory table in main process Initializated successful! \
            \n ---- The I/O queue is initialized successful! \
            \n ---- The buffer is initialized successful! \
            \n ---- Meta data space allocated successful! \
            \n");

     return 0;

}

void bench_testing(void)
{
     clock_t startTime,endTime;                        // Definition of timestamp
     /* workload a: insert only*/
     uint64_t written_data_size = 100000000*16;
     uint64_t written_data_num = written_data_size /(KEY_SIZE+VAL_SIZE);
     uint64_t record_point = written_data_num / 10;
     // printf("written_data_size:%lu, written_data_num:%lu record_point: %lu\n",written_data_size, written_data_num, record_point);

     startTime = clock();
     char key_buffer[KEY_SIZE];
     char value_buffer[VAL_SIZE];
     for(SKey i=1;i<=written_data_num;i++)
     {
          
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer, 0, VAL_SIZE);

          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((i >> (8 * j)) & 0xFF);
          }

          for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
          {
               value_buffer[KEY_SIZE - 1 - j] = static_cast<char>((i >> (8 * j)) & 0xFF);
          }

          InsertNode(key_buffer, value_buffer);

          if(i%record_point==0)
          {
               endTime = clock();
               printf("Read count:%d io_write:%d write2:%d erase:%d\n",reads,writes,writes_ram,resets);
               std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
          }
     }
     endTime = clock();
     printf("Read count:%d io_write:%d write2:%d erase:%d\n",reads,writes,writes_ram,resets);
     std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     // for(int i=0;i<255;i++)
     // {
     //      printf("block:%d: pointer:%lu\n",i,BlockWritePointers[i]/4);
     // }

     // uint64_t workb[1000010];
     // string x;
     // int index1=0;
     // ifstream inFile;
     // inFile.open("/home/femu/experiment1/code2/src/data/x2.txt");
     // if (!inFile) 
     // {
     //      std::cerr << "Unable to open file datafile.txt";
     //      exit(1);   // call system to stop
     // }
     // while (getline(inFile,x))     
     // {
     //      workb[index1++] = atoi(x.c_str());
     // }
     // inFile.close();

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
          //      ;
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

//      /* workload c: read only, 50% in it, 50% not in it */
//      startTime = clock();
//      reads = 0;
//      writes = 0;
//      resets = 0;
     
//      uint64_t b;
//      record_point = written_data_num/10/10;
//      for(uint64_t i=1;i<=written_data_num/10;i++)
//      {
//           memset(key_buffer, 0, KEY_SIZE);
//           srand48(time(NULL));
          
//           if(i%100<=20)
//           {
//                SKey k = 1+(rand()%written_data_num);
//                for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
//                {
//                     key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
//                }

//                b = k;
// #ifdef TIOCS_READ_DEBUG
//           printf("key: %lu\n", k);
// #endif         
//                if(b != Search(key_buffer))
//                {
//                     printf("Error in search\n");
//                     exit(0);
//                }
//           }
//           else
//           {
//                SKey k = written_data_num+(rand()%written_data_num);
//                for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
//                {
//                     key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
//                }
// #ifdef TIOCS_READ_DEBUG
//           printf("key: %lu\n", k);
// #endif         
//                Search(key_buffer);
//           }
          
//           if(i%record_point==0)
//           {
//                printf("Read count:%d write:%d write2:%d erase:%d\n",reads,writes,writefs,resets);
//                endTime = clock();
//                std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
//           } 
//      }
//      endTime = clock();
//      printf("Read count:%d write:%d write2:%d erase:%d\n",reads,writes,writefs,resets);
//      std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
          

     /* workload d: update heavy workload, 50% read, 50% update */
     startTime = clock();
     record_point = written_data_num/2/10;
     for(uint64_t i=1;i<=written_data_num/2;i++)
     {
          srand48(time(NULL));
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer,0, VAL_SIZE);
          SKey k = 1+(rand()%written_data_num);
          // if(i%2==0)
          // {
          //      for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          //      {
          //           key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
          //      }

          //      if(k!=Search(key_buffer) && k+1 != Search(key_buffer) )
          //      {
          //           printf("Error in search!\n");
          //           exit(0);
          //      }
          // }
          
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
          }
          for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
          {
               value_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k+1 >> (8 * j)) & 0xFF);
          }
          Update(key_buffer,value_buffer);
          
          if(i%record_point==0)
          {
               printf("Read count:%d io_write:%d write2:%d erase:%d\n",reads,writes,writes_ram,resets);
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
          } 
     }
     printf("Read count:%d io_write:%d write2:%d erase:%d\n",reads,writes,writes_ram,resets);
     endTime = clock();
     std::cout << "Total Time of workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
     printBlockInformation();
     printf("It's over!\n");
     // /* workload E: read mostly workload, 95% read, 5% update */
     // startTime = clock();
     // record_point = written_data_num/2/10;
     // for(int i=1;i<=written_data_num/2;i++)
     // {
     //      srand48(time(NULL));
     //      memset(key_buffer, 0, KEY_SIZE);
     //      memset(value_buffer,0, VAL_SIZE);
     //      if(i%100<95)
     //      {
     //           SKey k = 1+(rand()%written_data_num);
     //           for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
     //           {
     //                key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
     //           }
     //           Search(key_buffer);
     //      }
     //      else
     //      {
     //           SKey k = 1+(rand()%written_data_num);
     //           for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
     //           {
     //                key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
     //           }
     //           for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
     //           {
     //                value_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k+1 >> (8 * j)) & 0xFF);
     //           }
     //           Update(key_buffer,value_buffer);
     //      }
     //      if(i%record_point==0 )
     //      {
     //           printf("Read count:%d write:%d write2:%d erase:%d\n",reads,writes,writefs,resets);
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      } 
     // }
     // printf("Read count:%d write:%d write2:%d erase:%d\n",reads,writes,writefs,resets);
     // endTime = clock();
     // std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     // /* workload F: read latest workload, 95% read, 5% insert */
     // startTime = clock();
     // record_point = written_data_num/2/10;
     // for(int i=1;i<=written_data_num/2;i++)
     // {
     //      srand48(time(NULL));
     //      memset(key_buffer, 0, KEY_SIZE);
     //      memset(value_buffer,0, VAL_SIZE);
     //      if(i%100<95)
     //      {
     //           SKey k = 1+(rand()%written_data_num);
     //           for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
     //           {
     //                key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
     //           }
     //           Search(key_buffer);
     //      }
     //      else
     //      {
     //           SKey k = written_data_num+(rand()%written_data_num);

     //           for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
     //           {
     //                key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
     //           }

     //           for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
     //           {
     //                value_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k+1 >> (8 * j)) & 0xFF);
     //           }
     //           InsertNode(key_buffer,value_buffer);
     //      }
     //      if(i%record_point==0)
     //      {
     //           printf("Read count:%d write:%d write2:%d erase:%d\n",reads,writes,writefs,resets);
     //           endTime = clock();
     //           std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
     //      } 
     // }
     // printf("Read count:%d write:%d write2:%d erase:%d\n",reads,writes,writefs,resets);
     // endTime = clock();
     // std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     /* workload G: delete workload, 100% delete*/
     startTime = clock();
     record_point = written_data_num/5/10;
     for(int i=1;i<=written_data_num/5;i++)
     {
          srand48(time(NULL));
          SKey k = 1+(rand()%written_data_num);
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
          }
          Delete(key_buffer);
          if(i%record_point==0)
          {
               printf("Read count:%d io_write:%d write2:%d erase:%d\n",reads,writes,writes_ram,resets);
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";     
          }  
     }
     printf("Read count:%d io_write:%d write2:%d erase:%d\n",reads,writes,writes_ram,resets);
     endTime = clock();
     std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
     printBlockInformation();
     
}

void other_function(int count) 
{
    struct mymsgbuf buf;
    buf.mtype = 1;
     for (int i = 0; i < 3; ++i) 
     {
          /* send message */ 
          sprintf(buf.mtext, "Message %d from main process!", i); 
          //strcpy(buf.mtext, "Hello, child process!");
          printf("message id:%d\n",msgid);
          if(msgsnd(msgid, &buf, sizeof(buf.mtext), 0) == -1)
          {
               perror("Failed to send message");
               exit(1);
          }
          cout << "Sent message to child process: " << buf.mtext << endl;

          /* receive messages */ 
          memset(buf.mtext, 0, sizeof(buf.mtext));
          msgrcv(msgid, &buf, sizeof(buf.mtext), 0, 0);
          cout << "Received message from child process: " << buf.mtext << endl;
     }
}

int sub_process_init(void)
{
     pid_t pid = fork();

     if(pid == -1 )
     {
          perror("Failed to create child process");
          return -1;
     }
     else if(pid == 0)
     { 
          /* create message queue in child process */
          
          struct mymsgbuf buf;
          prctl(PR_SET_NAME, "GCinTiOCS ", 0, 0, 0);
          while (1)
          {    

               int ret = msgrcv(msgid, &buf, sizeof(buf.mtext), 1, 0);
               if(ret != -1)
               {
                    cout << "Received message from main process: " << buf.mtext <<" message id "<<msgid<< endl;
                    memset(buf.mtext, 0, sizeof(buf.mtext));
                    strcpy(buf.mtext, "Ack from child process!");
                    if(msgsnd(msgid, &buf, sizeof(buf.mtext), 0)!= -1)
                    {
                        cout << "Sent message to main process: " << buf.mtext << endl;
                    }
               }
               
               /* send messages in child process */
          }
          msgctl(msgid, IPC_RMID, NULL);
          exit(EXIT_SUCCESS);
     }
     else
     {
          return pid;
     }

     return 0;
     /**  
      * Pagedata = (TNCEntry *)malloc(bp->geo->l.nbytes);
      * if (Pagedata == nullptr)
      * {
      *   EMessageOutput("Index initialized failure!",1578);
      * }
      **/
}

void TiOCSInit(void)
{

     // pid_t child_pid;
     // int status;
     // key_t msg_key = ftok(".", 'm');
     // int flag;

     // msgid = msgget(msg_key, IPC_CREAT | 0666);
     // if(msgid == -1)
     // {
     //      perror("Failed to create message queue!\n");
     //      return ;
     // }


     // if((child_pid = sub_process_init()) == -1)
     // {
     //      printf("child process initialized failed!\n");
     // }
     // else
     // {
     //      printf("Child process created with PID:%d \n",child_pid);
     // }

     
     bool flag = print_init_info();
     if(flag == -1)
     {
          printf("Index initialized failed!\n");
          exit(1012);
     }
     
     bench_testing();


     // msgctl(msgid, IPC_RMID, NULL);
     // waitpid(child_pid, &status, 0);
}

void TiOCS_close(void)
{
     spdk_nvme_ctrlr_free_io_qpair(qpair);

     buffer_cleanup();

}