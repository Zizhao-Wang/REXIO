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
#include <set>
#include <sstream>
#include <vector>
#include <pthread.h>
#include "include/SysOutput.h"
#include "memlayer/MemTier.h"
#include "memlayer/syncstore.h"
#include "disklayer/io_manager.h"
#include "memlayer/parameters.h"


#define MAX_MSG_SIZE 1024

bool test11 = false;

volatile int msgid;

struct mymsgbuf 
{  
     long mtype;  
     char mtext[MAX_MSG_SIZE];  
};  

void bench_testing(void)
{

     clock_t startTime,endTime;                        // Definition of timestamp
     /* workload a: insert only*/
     uint64_t written_data_size = 100000000*16;
     uint64_t written_data_num = written_data_size /(KEY_SIZE+VAL_SIZE);
     uint64_t record_point = written_data_num / 10;
     startTime = clock();
     char key_buffer[KEY_SIZE];
     char value_buffer[VAL_SIZE];
     int error_bound = KEY_SIZE > 8 ? 8 : 0;

     for(SKey i=1;i<=written_data_num;i++)
     {
          
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer, 0, VAL_SIZE);

          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((i >> (8 * j)) & 0xFF);
          }

          for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
          {
               value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((i >> (8 * j)) & 0xFF);
          }

          InsertNode(key_buffer, value_buffer);

          if(i%record_point==0)
          {
               endTime = clock();
               printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
               std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
               printBlockInformation();
#ifdef NOT_SEPARATE_KV
               std::string experiment_name = "NOT_SEPARATE_FIXED_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data( "/home/data/Experiment1.txt", experiment_name, "workload_A", i);
#elif defined(NOT_SEPARATE_KV_variable)
               std::string experiment_name = "NOT_SEPARATE_VARIABLE_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data( "/home/data/Experiment2.txt", experiment_name, "workload_A", i);
#elif defined(SEPARATE_KV_FIXED_LOG)
               std::string experiment_name = "SEPARATE_FIXED_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data( "/home/data/Experiment3.txt", experiment_name, "workload_A", i);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
               std::string experiment_name = "SEPARATE_VARIABLE_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data( "/home/data/Experiment4.txt", experiment_name, "workload_A", i);
#endif
               
               io_resets = 0; 
          }
     }
     endTime = clock();
     printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
     std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

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
          
     reads_io = 0;
     writes_io = 0;
     writes_ram = 0;
     resets = 0;
     io_resets = 0;
     uint64_t* workd = new uint64_t[written_data_num/2+10];
     string x;
     int index1=0;
     ifstream inFile;
     std::ostringstream filenameStream;
     filenameStream << "/home/TiOCS/src/data/update_" << KEY_SIZE << "_" << VAL_SIZE << ".txt";
     std::string filename = filenameStream.str();
     std::cout << "update Filename: " << filename << std::endl;
     inFile.open(filename.c_str());
     if (!inFile) 
     {
          std::cerr << "Unable to open file datafile.txt";
          exit(1);   // call system to stop
     }
     while (getline(inFile,x))     
     {
          workd[index1++] = atoi(x.c_str());
     }
     inFile.close();
     /* workload d: update heavy workload, 50% read, 50% update */
     startTime = clock();
     record_point = written_data_num/2/10;
     
     for(uint64_t i=1;i<=written_data_num/2;i++)
     {
          srand48(time(NULL));
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer,0, VAL_SIZE);
          SKey k =  workd[i-1];
          // if(k == 8456562 )
          // {
          //      printf("get!!!!!!");
          // }
          // printf("Test: \n");
          // printf("key:%lu\n",k);
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
               key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
          }
          for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
          {
               value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k+1 >> (8 * j)) & 0xFF);
          }

          Update(key_buffer,value_buffer);
          
          if(i%record_point==0)
          {
               printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
               printBlockInformation();
#ifdef NOT_SEPARATE_KV
               std::string experiment_name = "NOT_SEPARATE_FIXED_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data("/home/data/Experiment1.txt", experiment_name, "workload_D", i);
#elif defined(NOT_SEPARATE_KV_variable)
               std::string experiment_name = "NOT_SEPARATE_VARIABLE_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data("/home/data/Experiment2.txt", experiment_name, "workload_D", i);
#elif defined(SEPARATE_KV_FIXED_LOG)
               std::string experiment_name = "SEPARATE_FIXED_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data("/home/data/Experiment3.txt", experiment_name, "workload_D", i);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
               std::string experiment_name = "SEPARATE_VARIABLE_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data("/home/data/Experiment4.txt", experiment_name, "workload_D", i);
#endif
               
               io_resets = 0;      
          }
     }
     printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
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
     test11 = true;
     reads_io = 0;
     writes_io = 0;
     writes_ram = 0;
     resets = 0;
     io_resets = 0;
     uint64_t* workg = new uint64_t[written_data_num+10];
     printf("wrriten_data_num:%lu wrritten_data_num:%lu\n",written_data_num,written_data_num/5+10);
     index1=0;
     clearBufferLog();
     filenameStream.str("");
     filenameStream.clear();
     filenameStream << "/home/TiOCS/src/data/delete_" << KEY_SIZE << "_" << VAL_SIZE << ".txt";
     filename = filenameStream.str();
     std::cout << "delete Filename: " << filename << std::endl;
     inFile.open(filename.c_str());
     // inFile.open("/home/TiOCS/src/data/delete.txt");
     if (!inFile) 
     {
          std::cerr << "Unable to open file datafile.txt"<<endl;
          exit(1);   // call system to stop
     }
     while (getline(inFile,x))     
     {
          workg[index1++] = atoi(x.c_str());
     }
     printf("index1:%d\n",index1);
     inFile.close();
     /* workload G: delete workload, 100% delete*/
     startTime = clock();
     record_point = written_data_num/5/10;
     for(int i=1;i<=written_data_num/5;i++)
     {
          srand48(time(NULL));
          SKey k = workg[i-1];
          // printf("key:%lu\n",k);
          memset(key_buffer, 0, KEY_SIZE);
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
          }
          // printf("key_buffer:%lu\n",big_endian2little_endian(key_buffer,KEY_SIZE));
          Delete(key_buffer);
          if(i%record_point==0)
          {
               printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
               endTime = clock();
               std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
               printBlockInformation();
#ifdef NOT_SEPARATE_KV
               std::string experiment_name = "NOT_SEPARATE_FIXED_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data("/home/data/Experiment1txt", experiment_name, "workload_G", i);
#elif defined(NOT_SEPARATE_KV_variable)
               std::string experiment_name = "NOT_SEPARATE_VARIABLE_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data("/home/data/Experiment2.txt", experiment_name, "workload_G", i);
#elif defined(SEPARATE_KV_FIXED_LOG)
               std::string experiment_name = "SEPARATE_FIXED_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data("/home/data/Experiment3.txt", experiment_name, "workload_G", i);
#elif defined(SEPARATE_KV_VARIABLE_LOG)
               std::string experiment_name = "SEPARATE_VARIABLE_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
               write_data("/home/data/Experiment4.txt", experiment_name, "workload_G", i);
#endif
               
               io_resets = 0;     
          }  
     }
     printf("Read count:%d io_write:%d write2:%d erase:%d block resets:%d\n",reads_io,writes_io,writes_ram,resets,io_resets);
     endTime = clock();
     std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
     printBlockInformation();
     delete [] workg;
     delete [] workd;
     countBufferLog();
     printf("It's over!\n");
     
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
      * Pagedata = (key_value_entry *)malloc(bp->geo->l.nbytes);
      * if (Pagedata == nullptr)
      * {
      *   EMessageOutput("Index initialized failure!",1578);
      * }
      **/
}

void TiOCSInit(void)
{

     pthread_t io_thread; 
     int status;
     key_t msg_key = ftok(".", 'm');

     // Create io_management thread
     if(pthread_create(&io_thread, NULL, io_management, NULL) != 0)
     {
          fprintf(stderr,"Thread creation failed!\n");
          return;
     }
     else
     {
          printf("\033[1;32mI/O Thread successfully created!\033[0m\n");
     }

     bool flag = print_init_info();
     if(flag == false)
     {
          printf("Index initialized failed!\n");
          exit(1012);
     }

     bench_testing();
     pthread_join(io_thread, NULL);
}

void TiOCS_close(void)
{
     spdk_nvme_ctrlr_free_io_qpair(qpair);

     buffer_cleanup();

}