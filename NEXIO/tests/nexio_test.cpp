#include "nexio_test.h"
#include "zipf_generator.h"
#include "include/pre_definition.h"
#include "memlayer/MemTier.h"
#include "disklayer/io_manager.h"
#include "memlayer/asyncstore.h"
#include "memlayer/syncstore.h"
#include "ycsb_test.h"
#include <gflags/gflags.h>

DEFINE_string(benchmarks, " ", "define the type of benchmark");
DEFINE_int64(num,             1000000,  "Number of key/values to place in database");
DEFINE_int64(range,           -1,       "key range space");
DEFINE_int64(reads,           -1,       "Number of read operations to do.  If negative, do FLAGS_num reads.");
DEFINE_int32(value_size,      100,      "Size of each value");
DEFINE_bool(histogram,        false,    "Print histogram of operation timings");
DEFINE_bool(print_wa,         false,    "Print write amplification every stats interval");
DEFINE_int64(write_buffer_size, 2097152,"Number of bytes to buffer in all memtables before compacting");
DEFINE_int32(block_size,      4096,     "Approximate size of user data packed per block before compression.");
DEFINE_int64(cache_size,      8 << 20,  "Number of bytes to use as a cache of uncompressed data,Negative means use default settings");
DEFINE_int32(stats_interval,  1000000,  "Interval that print status");
DEFINE_int32(log_buffer_size, 65536,    "");
DEFINE_double(zipf_exponent, 1.0, "exponent parameter for the Zipf distribution");


char key_buffer[KEY_SIZE];
char value_buffer[VAL_SIZE];
int error_bound = KEY_SIZE > 8 ? 8 : 0;
uint64_t user_input_bytes = 0;
double total_operation_time = 0.0;  
uint64_t last_operation_time = 0; 
double total_latency = 0.0; 
double max_latency = 0.0; 
int j = 1;
double min_latency = std::numeric_limits<double>::max();
double throughput = 0.0;
double average_latency = 0;


void bench_testing(void)
{

     // ZipfGenerator generator(FLAGS_num, FLAGS_zipf_exponent);
    
     clock_t startTime,endTime;                        // Definition of timestamp
     /* workload a: insert only*/
     uint64_t written_data_size = 100000000*16;
     uint64_t written_data_num = written_data_size /(KEY_SIZE+VAL_SIZE);
     uint64_t record_point = FLAGS_num /10 ;
     
     uint64_t data_point = 0;  
     bool is_random = false;
     if (FLAGS_benchmarks.find("fillrandom") != std::string::npos) {
          is_random = false;
     }
     startTime = clock();
     memset(key_buffer, 0, KEY_SIZE);
     memset(value_buffer, 0, VAL_SIZE);
     for(uint64_t i=1;i<=FLAGS_num;i++)
     {
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer, 0, VAL_SIZE);
          data_point = i; // is_random?(generator.next()%FLAGS_range):i;
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }

          for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
          {
               value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }

          user_input_bytes = user_input_bytes +(KEY_SIZE + VAL_SIZE);
          uint64_t operation_start_time = clock();

          DEBUG_PRINT("data point:%lu  key:%lu \n",data_point,big_endian2little_endian(key_buffer,KEY_SIZE));
          // printf("%lu  datapoint:%lu \n",i,data_point);
          // InsertNode(key_buffer, value_buffer);

          uint64_t operation_end_time = clock();
          double current_latency = (double)(operation_end_time - operation_start_time) / CLOCKS_PER_SEC;
          total_latency += current_latency;
          if(current_latency > max_latency){
               max_latency = current_latency;
          }
          if(current_latency < min_latency){
               min_latency = current_latency;
          } 
          total_operation_time += current_latency;

          
          if(i%record_point==0)
          {
               printBlockInformation();
               printf("%d/10 has been completed!\n",j);
               j++;
          }
     }
     // PrintHashTable();
     endTime = clock();
     throughput = written_data_num / ((double)(endTime - startTime) / CLOCKS_PER_SEC);
     average_latency = total_latency / written_data_num;

     fprintf(stderr,"arrival!!!!!!\n");

     fprintf(stdout,"\n\033[1;34mStore Performance Metrics:\033[0m\n");
     fprintf(stdout,"------------------------------\n");
     fprintf(stdout,"Throughput:                 %0.3lf ops/sec\n", throughput);
     fprintf(stdout,"Average Latency:            %0.3lf ms\n", average_latency * 1000); // Convert to milliseconds for better readability
     fprintf(stdout,"Max Latency:                %0.3lf ms\n", max_latency * 1000);
     fprintf(stdout,"Min Latency:                %0.3lf ms\n", min_latency * 1000);
     fprintf(stdout,"The Write Amplification:    %0.3lf\n", (double)total_write_bytes/user_input_bytes); // Assuming you have a variable called write_amplification
     fprintf(stdout,"------------------------------\n");
     std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";


     ycsb_read_heavy(written_data_num);

     ycsb_read_only(written_data_num);

     ycsb_update_heavy(written_data_num);

     /* workload d: update heavy workload, 50% read, 50% update */
     total_write_bytes = 0;
     user_input_bytes = 0;
     startTime = clock();
     record_point = written_data_num/2/10;
     
     for(uint64_t i=1;i<=written_data_num/2;i++)
     {
          srand48(time(NULL));
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer,0, VAL_SIZE);
          uint64_t k;
          printf("Test: \n");
          printf("key:%lu\n",k);
          if(i%2==0)
          {
               for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
               {
                    key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
               }

               if(k!=Search(key_buffer) && k+1 != Search(key_buffer) )
               {
                    printf("Error in search!\n");
                    exit(0);
               }
          }
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
          }
          for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j)
          {
               value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k+1 >> (8 * j)) & 0xFF);
          }

          user_input_bytes = user_input_bytes +(KEY_SIZE + VAL_SIZE);
          uint64_t operation_start_time = clock();
          Update(key_buffer,value_buffer);

          uint64_t operation_end_time = clock();
          double current_latency = (double)(operation_end_time - operation_start_time) / CLOCKS_PER_SEC;
          total_latency += current_latency;
          if(current_latency > max_latency){
               max_latency = current_latency;
          }
          if(current_latency < min_latency){
               min_latency = current_latency;
          } 
          total_operation_time += current_latency;
          
          
          if(i%record_point==0)
          {
               printBlockInformation();     
          }
     }

     endTime = clock();
     throughput = written_data_num / ((double)(endTime - startTime) / CLOCKS_PER_SEC);
     average_latency = total_latency / written_data_num;
     printf("\n\033[1;34mStore Performance Metrics:\033[0m\n");
     printf("------------------------------\n");
     printf("Throughput:                 %0.3lf ops/sec\n", throughput);
     printf("Average Latency:            %0.3lf ms\n", average_latency * 1000); // Convert to milliseconds for better readability
     printf("Max Latency:                %0.3lf ms\n", max_latency * 1000);
     printf("Min Latency:                %0.3lf ms\n", min_latency * 1000);
     printf("The Write Amplification:    %0.3lf\n", (double)total_write_bytes/user_input_bytes); // Assuming you have a variable called write_amplification
     printf("------------------------------\n\n");
     std::cout << "Total Time of workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
     printBlockInformation();
     printf("It's over!\n");


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

     // total_write_bytes = 0;
     // uint64_t* workg = new uint64_t[written_data_num+10];
     // printf("wrriten_data_num:%lu wrritten_data_num:%lu\n",written_data_num,written_data_num/5+10);
     // clearBufferLog();
     // filenameStream.str("");
     // filenameStream.clear();
     // filenameStream << "/home/TiOCS/src/data/delete_" << KEY_SIZE << "_" << VAL_SIZE << ".txt";
     // filename = filenameStream.str();
     // std::cout << "delete Filename: " << filename << std::endl;
     // inFile.open(filename.c_str());
     // // inFile.open("/home/TiOCS/src/data/delete.txt");
     // if (!inFile) 
     // {
     //      std::cerr << "Unable to open file datafile.txt"<<std::endl;
     //      exit(1);   // call system to stop
     // }
     // while (getline(inFile,x))     
     // {
     //      workg[index1++] = atoi(x.c_str());
     // }
     // printf("index1:%d\n",index1);
     // inFile.close();
     /* workload G: delete workload, 100% delete*/
//      startTime = clock();
//      record_point = written_data_num/5/10;
//      for(int i=1;i<=written_data_num/5;i++)
//      {
//           srand48(time(NULL));
//           uint64_t k = workg[i-1];
//           // printf("key:%lu\n",k);
//           memset(key_buffer, 0, KEY_SIZE);
//           for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
//           {
//                key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
//           }
//           // printf("key_buffer:%lu\n",big_endian2little_endian(key_buffer,KEY_SIZE));
//           Delete(key_buffer);
//           if(i%record_point==0)
//           {
//                endTime = clock();
//                std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
//                printBlockInformation();
// #ifdef NOT_SEPARATE_KV
//                std::string experiment_name = "NOT_SEPARATE_FIXED_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
//                write_data("/home/data/Experiment1txt", experiment_name, "workload_G", i);
// #elif defined(NOT_SEPARATE_KV_variable)
//                std::string experiment_name = "NOT_SEPARATE_VARIABLE_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
//                write_data("/home/data/Experiment2.txt", experiment_name, "workload_G", i);
// #elif defined(SEPARATE_KV_FIXED_LOG)
//                std::string experiment_name = "SEPARATE_FIXED_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
//                write_data("/home/data/Experiment3.txt", experiment_name, "workload_G", i);
// #elif defined(SEPARATE_KV_VARIABLE_LOG)
//                std::string experiment_name = "SEPARATE_VARIABLE_KEY_" + std::to_string(KEY_SIZE) + "_VALUE_" + std::to_string(VAL_SIZE);
//                write_data("/home/data/Experiment4.txt", experiment_name, "workload_G", i);
// #endif   
//           }  
//      }
//      endTime = clock();
//      std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
//      printBlockInformation();
//      delete [] workg;
//      // delete [] workd;
//      countBufferLog();
//      printf("It's over!\n");
}
