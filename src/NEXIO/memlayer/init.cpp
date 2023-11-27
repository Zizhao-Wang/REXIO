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

#include "include/init.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>
#include <pthread.h>
#include "include/sys_output.h"
#include "memlayer/MemTier.h"
#include "memlayer/syncstore.h"
#include "disklayer/io_manager.h"
#include "memlayer/parameters.h"
#include "include/buffer.h"
#include "include/spdk_env_init.h"
#include "disklayer/write_buffer.h"
#include "disklayer/io_manager/io_thread.h"


#define MAX_MSG_SIZE 1024


volatile int msgid;

struct mymsgbuf 
{  
     long mtype;  
     char mtext[MAX_MSG_SIZE];  
};  

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
          std::cout << "Sent message to child process: " << buf.mtext << std::endl;

          /* receive messages */ 
          memset(buf.mtext, 0, sizeof(buf.mtext));
          msgrcv(msgid, &buf, sizeof(buf.mtext), 0, 0);
          std::cout << "Received message from child process: " << buf.mtext << std::endl;
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
                    std::cout << "Received message from main process: " << buf.mtext <<" message id "<<msgid<< std::endl;
                    memset(buf.mtext, 0, sizeof(buf.mtext));
                    strcpy(buf.mtext, "Ack from child process!");
                    if(msgsnd(msgid, &buf, sizeof(buf.mtext), 0)!= -1)
                    {
                        std::cout << "Sent message to main process: " << buf.mtext << std::endl;
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
     
     int status;

     // Create io_management thread; Initialize the mutexes and condition variables
     fprintf(stdout, "\x1b[34m\n=============== NEXIO initialization start ===============\x1b[0m\n");
     if (init_mutex() != 0) {
          printf("\033[0;31m[ERROR]\033[0m Failed to initialize mutexes and condition variables.\n");
          return; // or handle the error as appropriate for your application
     } else {
          fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m Mutexes and condition variables successfully initialized!\033[0m\n");
     }

     // Create the I/O management thread
     if (pthread_create(&io_thread, NULL, io_management, NULL) != 0) {
          printf("\033[0;31m[ERROR]\033[0m Thread creation failed!\n");
          return; // or handle the error as appropriate for your application
     } else {
          fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m I/O Thread successfully created!\033[0m\n");
     }


     int flag = initialize_system();
     if (flag == -1) {
          fprintf(stderr, "\033[0;31m[ERROR]\033[0m System initialization failed!\n");
          exit(1012);
     } else {
          fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m All components in system successfully initialized!\033[0m");
     }
     fprintf(stdout, "\x1b[34m\n=============== NEXIO Initialization completed ===============\x1b[0m\n");
}

void TiOCS_close(void)
{
     spdk_nvme_ctrlr_free_io_qpair(qpair);

     combined_kv_buffer_cleanup();

#ifdef NVME_SSD_CONTROLLER
     free_nvme_device_info();
#endif

     if(keep_running){
          keep_running = false;
          pthread_cond_broadcast(&io_cond); // 通知所有等待条件变量的线程
          pthread_join(io_thread, NULL);
          fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m Successfully recycling the IO thread.\n");
     }    
}