#include "ycsb_test.h"

#include <vector>
#include <deque>

#include "nexio_test.h"
#include "memlayer/MemTier.h"
#include "data_distribution.h"
#include "utility/types.h"


void ycsb_testing(void)
{

     // ZipfGenerator generator(FLAGS_num, FLAGS_zipf_exponent);
    
     clock_t startTime,endTime;                        // Definition of timestamp
     uint64_t written_data_size = 100000000*16;
     uint64_t written_data_num = written_data_size /(KEY_SIZE+FLAGS_value_size);
     uint64_t record_point = 1000000;
     
     char key_buffer[KEY_SIZE];
     char *value_buffer = new char[FLAGS_value_size];

     uint64_t data_point = 0; 
     startTime = clock();
     memset(key_buffer, 0, KEY_SIZE);
     memset(value_buffer, 0, FLAGS_value_size);
     for(uint64_t i=1;i<=FLAGS_num;i++)
     {
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer, 0, FLAGS_value_size);
          data_point = i; // is_random?(generator.next()%FLAGS_range):i;
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j) 
          {
               key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }

          for (size_t j = 0; j < sizeof(uint64_t) && j < FLAGS_value_size; ++j)
          {
               value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }

          user_input_bytes = user_input_bytes +(KEY_SIZE + FLAGS_value_size);
          uint64_t operation_start_time = clock();

          DEBUG_PRINT("data point:%lu  key:%lu \n",data_point,big_endian2little_endian(key_buffer,KEY_SIZE));
          // printf("%lu  datapoint:%lu \n",i,data_point);
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

          
          if(i%record_point==0)
          {
               printf("%d million has been completed!\n",j);
               j++;
          }
     }
     endTime = clock();
     throughput = FLAGS_num / ((double)(endTime - startTime) / CLOCKS_PER_SEC);
     average_latency = total_latency / FLAGS_num;

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

     ycsb_read_heavy(FLAGS_num/10);

     ycsb_read_only(FLAGS_num/10);

     ycsb_update_heavy(FLAGS_num/10);

     ycsb_read_latest(FLAGS_num/10);

}



void ycsb_update_heavy(uint64_t modified_data_num) {

     char key_buffer[KEY_SIZE];
     char *value_buffer = new char[FLAGS_value_size];
     total_latency = 0.0, max_latency = 0.0, min_latency = std::numeric_limits<double>::max();
     total_write_bytes = 0, user_input_bytes = 0;
     clock_t startTime = clock();
     uint64_t record_point = modified_data_num / 2 / 10;

     uint64_t data_point = 0;
     for (uint64_t i = 1; i <= modified_data_num; i++){
          if(i%2==0){
               for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j){
                    key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
               }

               if(data_point!=Search(key_buffer) && data_point+1 != Search(key_buffer)){
                    printf("Error in search!\n");
                    exit(0);
               }
          }else{
               for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j){
                    key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
               }
               for (size_t j = 0; j < sizeof(uint64_t) && j < FLAGS_value_size; ++j){
                    value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point+1 >> (8 * j)) & 0xFF);
               }

               user_input_bytes = user_input_bytes +(KEY_SIZE + FLAGS_value_size);
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
          }

          if(i%record_point == 0){
               clock_t endTime = clock();
               throughput = modified_data_num / ((double)(endTime - startTime) / CLOCKS_PER_SEC);
               average_latency = total_latency / modified_data_num;
               printf("\n\033[1;34mStore Performance Metrics:\033[0m\n");
               printf("------------------------------\n");
               printf("Throughput:                 %0.3lf ops/sec\n", throughput);
               printf("Average Latency:            %0.3lf ms\n", average_latency * 1000); // Convert to milliseconds for better readability
               printf("Max Latency:                %0.3lf ms\n", max_latency * 1000);
               printf("Min Latency:                %0.3lf ms\n", min_latency * 1000);
               printf("The Write Amplification:    %0.3lf\n", (double)total_write_bytes/user_input_bytes); // Assuming you have a variable called write_amplification
               printf("------------------------------\n\n");
               std::cout << "Total Time of workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
          }
          
     }
}


void ycsb_read_heavy(uint64_t modified_data_num) 
{
     char key_buffer[KEY_SIZE];
     char *value_buffer = new char[FLAGS_value_size];
     total_latency = 0.0, max_latency = 0.0, min_latency = std::numeric_limits<double>::max();
     total_write_bytes = 0, user_input_bytes = 0;
     clock_t startTime = clock();
     uint64_t record_point = modified_data_num / 2 / 10;

     uint64_t data_point =0;
     for (uint64_t i = 1; i <= modified_data_num; i++){
          if(i%100<=95){
               for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j){
                    key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
               }

               if(data_point!=Search(key_buffer) && data_point+1 != Search(key_buffer)){
                    printf("Error in search!\n");
                    exit(0);
               }
          }else{
               for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j){
                    key_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
               }
               for (size_t j = 0; j < sizeof(uint64_t) && j < FLAGS_value_size; ++j){
                    value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point+1 >> (8 * j)) & 0xFF);
               }

               user_input_bytes = user_input_bytes +(KEY_SIZE + FLAGS_value_size);
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
          }

          if(i%record_point == 0){
               clock_t endTime = clock();
               throughput = modified_data_num / ((double)(endTime - startTime) / CLOCKS_PER_SEC);
               average_latency = total_latency / modified_data_num;
               printf("\n\033[1;34mStore Performance Metrics:\033[0m\n");
               printf("------------------------------\n");
               printf("Throughput:                 %0.3lf ops/sec\n", throughput);
               printf("Average Latency:            %0.3lf ms\n", average_latency * 1000); // Convert to milliseconds for better readability
               printf("Max Latency:                %0.3lf ms\n", max_latency * 1000);
               printf("Min Latency:                %0.3lf ms\n", min_latency * 1000);
               printf("The Write Amplification:    %0.3lf\n", (double)total_write_bytes/user_input_bytes); // Assuming you have a variable called write_amplification
               printf("------------------------------\n\n");
               std::cout << "Total Time of workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
          }
          
     }
}


void ycsb_read_only(uint64_t modified_data_num) 
{
     char key_buffer[KEY_SIZE];
     char *value_buffer = new char[FLAGS_value_size];
     total_latency = 0.0, max_latency = 0.0, min_latency = std::numeric_limits<double>::max();
     total_write_bytes = 0, user_input_bytes = 0;
     clock_t startTime = clock();
     uint64_t record_point = modified_data_num / 2 / 10;

     uint64_t data_point = 0;
     for (uint64_t i = 1; i <= modified_data_num; i++){
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j){
               key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }
          uint64_t operation_start_time = clock();
          if(data_point!=Search(key_buffer) && data_point+1 != Search(key_buffer)){
               printf("Error in search!\n");
               exit(0);
          }
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

          if(i%record_point == 0){
               clock_t endTime = clock();
               throughput = modified_data_num / ((double)(endTime - startTime) / CLOCKS_PER_SEC);
               average_latency = total_latency / modified_data_num;
               printf("\n\033[1;34mStore Performance Metrics:\033[0m\n");
               printf("------------------------------\n");
               printf("Throughput:                 %0.3lf ops/sec\n", throughput);
               printf("Average Latency:            %0.3lf ms\n", average_latency * 1000); // Convert to milliseconds for better readability
               printf("Max Latency:                %0.3lf ms\n", max_latency * 1000);
               printf("Min Latency:                %0.3lf ms\n", min_latency * 1000);
               printf("The Write Amplification:    %0.3lf\n", (double)total_write_bytes/user_input_bytes); // Assuming you have a variable called write_amplification
               printf("------------------------------\n\n");
               std::cout << "Total Time of read only workload: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
          }
     }
}




void ycsb_read_latest(size_t operationsCount) 
{ 
     TraceUniform keyGenerator(1, operationsCount);
     std::deque<uint64_t> recentKeys;
     uint64_t data_point = keyGenerator.Next();
     char key_buffer[KEY_SIZE];
     char *value_buffer = new char[FLAGS_value_size];


     for (size_t i = 0; i < operationsCount/2; ++i){
          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer, 0, FLAGS_value_size);
          data_point= keyGenerator.Next();
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j){
               key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }
          for(size_t j = 0; j < sizeof(uint64_t) && j < FLAGS_value_size; ++j){
               value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point+1 >> (8 * j)) & 0xFF);
          }
          if(rand() % 100 < 50){
               InsertNode(key_buffer, value_buffer);
          }else{
               Update(key_buffer, value_buffer);
          }

          recentKeys.push_back(data_point);


          if(recentKeys.size() > operationsCount) { 
               recentKeys.pop_front();
          }
     }

     for (size_t i = 0; i < operationsCount; ++i) {


          memset(key_buffer, 0, KEY_SIZE);
          memset(value_buffer, 0, FLAGS_value_size);

          if ((rand() % 100) < 95) {
               if (!recentKeys.empty()) 
               {
                    size_t randomIndex = rand() % recentKeys.size();
                    uint64_t keyToRead = recentKeys[randomIndex];
                    for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j){
                         key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((keyToRead >> (8 * j)) & 0xFF);
                    }                    
                    uint64_t value = Search(key_buffer); 
               }
          }else {
               data_point = keyGenerator.Next();
               InsertNode(key_buffer, value_buffer);
               recentKeys.push_back(data_point);
               if (recentKeys.size() > operationsCount) {
                    recentKeys.pop_front();
               }
        }
    }
}
