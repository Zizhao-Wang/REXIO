#include "nexio_test.h"
#include <random> 
#include "include/pre_definition.h"
#include "memlayer/MemTier.h"
#include "disklayer/io_manager.h"
#include "memlayer/asyncstore.h"
#include "memlayer/syncstore.h"
#include "ycsb_test.h"
#include "data_distribution.h"
#include "utility/types.h"






int error_bound = KEY_SIZE > 8 ? 8 : 0;
uint64_t user_input_bytes = 0;
double total_operation_time = 0.0;  
uint64_t last_operation_time = 0; 
double total_latency = 0.0; 
double max_latency = 0.0; 
int j = 1;
double min_latency = std::numeric_limits<double>::max();
double throughput = 0.0;
double throughput2 = 0.0;
double average_latency = 0;


DEFINE_string(benchmarks, " ", "define the type of benchmark");
DEFINE_int64(num,             1000000,  "Number of key/values to place in database");
DEFINE_int64(range,           -1,       "key range space");
DEFINE_int64(reads,           -1,       "Number of read operations to do.  If negative, do FLAGS_num reads.");
DEFINE_int64(value_size,      100,      "Size of each value");
DEFINE_bool(histogram,        false,    "Print histogram of operation timings");
DEFINE_bool(print_wa,         false,    "Print write amplification every stats interval");
DEFINE_int64(write_buffer_size, 2097152,"Number of bytes to buffer in all memtables before compacting");
DEFINE_int32(block_size,      4096,     "Approximate size of user data packed per block before compression.");
DEFINE_int32(buckets,      8,     "number of buckets in hashing table");
DEFINE_int64(cache_size,      8 << 20,  "Number of bytes to use as a cache of uncompressed data,Negative means use default settings");
DEFINE_int32(stats_interval,  1000000,  "Interval that print status");
DEFINE_int32(log_buffer_size, 65536,    "");
DEFINE_double(zipf_exponent, 1.0, "exponent parameter for the Zipf distribution");
DEFINE_string(pci_address, "0000:81:00.0", "SSD address in PCIe port");


void bench_testing(void)
{
     Trace * trace = new TraceUniform(1000 + 1* 345);

     uint64_t done_ops = 0; 
     uint64_t next_report = 100; 
     char key_buffer[KEY_SIZE];
     char *value_buffer = new char[FLAGS_value_size];

     clock_t startTime,endTime;                        // Definition of timestamp
     /* workload a: insert only*/
     uint64_t written_data_size = 100000000*16;
     uint64_t written_data_num = written_data_size /(KEY_SIZE+FLAGS_value_size);
     uint64_t record_point = 1000000;
     FLAGS_stats_interval = FLAGS_num / 10;
     
     uint64_t data_point = 0;  
     bool is_random = false;
     if (FLAGS_benchmarks.find("fillrandom") != std::string::npos) {
          is_random = true;
     }
     startTime = clock();
     fprintf(stderr, "start benchmark:\n");

     // uint64_t data_point2;
     // std::mt19937 rng(std::random_device{}());
     // std::uniform_int_distribution<uint64_t> dist(1, UINT64_MAX);
     // for(uint64_t i =1;i<100;i++)
     // {
     //      data_point = dist(rng);
     //      data_point2 = dist(rng);

     //      for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) {
     //           key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
     //      }

     //      for (size_t j = 0; j < sizeof(uint64_t) && j < FLAGS_value_size; ++j){
     //           value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point2 >> (8 * j)) & 0xFF);
     //      }

     //      int sse_result  = compareWithSSE(key_buffer, value_buffer, KEY_SIZE);
     //      int memcmp_result = memcmp(key_buffer, value_buffer, KEY_SIZE);

     //      if(memcmp_result<0)
     //      {
     //           if(sse_result<0)
     //           {
     //                printf("true!\n");
     //           }
     //           else{
     //                printf("error\n");
     //           }
     //      }
     //      if(memcmp_result>0){
     //           if(sse_result>0)
     //           {
     //                printf("true!\n");
     //           }
     //           else{
     //                printf("error\n");
     //           }
     //      }
     //      if(memcmp_result==0){
     //           if(sse_result==0)
     //           {
     //                printf("true!\n");
     //           }
     //           else{
     //                printf("error\n");
     //           }
     //      }
          
     // }

     // exit(0);

     for(uint64_t i=1;i<=FLAGS_num;i++)
     {
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer, 0, value_size);
          data_point = is_random?(trace->Next()%FLAGS_range):i;
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }

          for (size_t j = 0; j < sizeof(uint64_t) && j < value_size; ++j)
          {
               value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }

          user_input_bytes += (KEY_SIZE + value_size);
          DEBUG_PRINT("data point:%lu  key:%lu \n",data_point,big_endian2little_endian(key_buffer,KEY_SIZE));

          uint64_t operation_start_time = clock();
          InsertNode(key_buffer, value_buffer);
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

          // if (current_latency*1000000 > 20000) {
          //      fprintf(stderr, "long op: %.1f micros%30s\r", current_latency*1e6, "");
          //      fflush(stderr);
          // }

          done_ops++;
          // if (done_ops >= next_report) {
          //      fprintf(stderr, "... finished %llu ops%30s\r", (unsigned long long)done_ops, ""); 
          //      fflush(stderr);

          //      if      (next_report < 1000)   next_report += 100;
          //      else if (next_report < 5000)   next_report += 500;
          //      else if (next_report < 10000)  next_report += 1000;
          //      else if (next_report < 50000)  next_report += 5000;
          //      else if (next_report < 100000) next_report += 10000;
          //      else if (next_report < 500000) next_report += 50000;
          //      else                           next_report += 100000;
          // }

          if(i % FLAGS_stats_interval == 0 )
          {
               endTime = clock();
               // throughput = i / total_operation_time;
               // average_latency = total_latency / i;
               // char timeStr[100];
               // time_t now = time(NULL);
               // strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
               // fprintf(stdout,"\nStore Performance Metrics\n");
               // fprintf(stdout,"------------------------------\n");
               // fprintf(stdout,"[%s] ",timeStr);
               // fprintf(stdout, "(%lu,%lu) ops ", i-FLAGS_stats_interval, i);
               // fprintf(stdout,"Total execution time: %0.3lf s\n", total_operation_time);
               // fprintf(stdout,"Data Throughput (MB/s): %0.3lf ops/sec  ", throughput);
               // throughput2 = (user_input_bytes / 1024.0 / 1024.0) / total_operation_time;
               // fprintf(stdout,"Operation Throughput (ops/sec): %0.3lf MB/s\n", throughput2);
               // fprintf(stdout,"Average Latency: %0.3lf ms  ", average_latency * 1000); // Convert to milliseconds for better readability
               // fprintf(stdout,"Max Latency: %0.3lf ms  ", max_latency * 1000);
               // fprintf(stdout,"Min Latency: %0.3lf ms\n", min_latency * 1000);
               // fprintf(stdout,"The Write Amplification: %0.3lf\n", (double)total_write_bytes/user_input_bytes); // Assuming you have a variable called write_amplification
               // fprintf(stdout,"------------------------------\n");
               // std::cout << "Convert and Find Bucket Time: " << convert_and_find_bucket_time.count() << " ms\n";
               // std::cout << "Search Time: " << search_time.count() << " ms\n";
               // std::cout << "Insert and Write Time: " << insert_and_write_time.count() << " ms\n";
               // std::cout << "split Time: " << split_time.count() << " ms\n";
               fflush(stdout);
          }
     }
     // PrintHashTable();
     endTime = clock();
     throughput = FLAGS_num / ((double)(endTime - startTime) / CLOCKS_PER_SEC);
     average_latency = total_latency / FLAGS_num;
     char timeStr[100];
     time_t now = time(NULL);
     strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
     fprintf(stdout,"\n Overall Performance of database\n");
     fprintf(stdout,"------------------------------\n");
     fprintf(stdout,"[%s] ",timeStr);
     fprintf(stdout, "(1,%lu) ops ", FLAGS_num);
     fprintf(stdout,"Total execution time: %0.3lf s\n", total_operation_time);
     fprintf(stdout,"Data Throughput (MB/s): %0.3lf ops/sec  ", throughput);
     throughput2 = (user_input_bytes / 1024.0 / 1024.0) / total_operation_time;
     fprintf(stdout,"Operation Throughput (ops/sec): %0.3lf MB/s\n", throughput2);
     fprintf(stdout,"Average Latency: %0.3lf ms  ", average_latency * 1000); // Convert to milliseconds for better readability
     fprintf(stdout,"Max Latency: %0.3lf ms  ", max_latency * 1000);
     fprintf(stdout,"Min Latency: %0.3lf ms\n", min_latency * 1000);
     fprintf(stdout,"The Write Amplification: %0.3lf\n", (double)total_write_bytes/user_input_bytes); // Assuming you have a variable called write_amplification
     fprintf(stdout,"------------------------------\n");
     fflush(stdout);
     process_pending_spdk_io();
     // ycsb_read_heavy(FLAGS_num/10);

     // ycsb_read_only(FLAGS_num/10);

     // ycsb_update_heavy(FLAGS_num/10);

     // ycsb_read_latest(FLAGS_num/10);

     /* workload d: update heavy workload, 50% read, 50% update */
     total_write_bytes = 0;
     user_input_bytes = 0;
     startTime = clock();
     record_point = FLAGS_num/2/10;
     int found_count = 0; // Counter for the number of times the key is found
     int not_found_count = 0; // Counter for the number of times the key is not found
     int total_attempts = 0; // Total number of search attempts
     
     for(uint64_t i=1;i<=FLAGS_num;i++)
     {
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer,0, FLAGS_value_size);
          uint64_t k = i;
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((k >> (8 * j)) & 0xFF);
          }
          k = Search(key_buffer);

          if (k == i) {
               found_count++; 
          } else {
               printf("Error: key:%lu value:%lu\n", i, k);
               exit(1);  
               not_found_count++; 
          }
          total_attempts++; 
          

          if(record_point!=0 && i%record_point==0)
          {
               printBlockInformation();     
          }
     }
     // Calculate the find rate
     double found_rate = (double)found_count / total_attempts * 100;

     // Print the statistical results
     printf("Found count: %d\n", found_count);
     printf("Not found count: %d\n", not_found_count);
     printf("Total attempts: %d\n", total_attempts);
     printf("Find rate: %.2f%%\n", found_rate);

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


