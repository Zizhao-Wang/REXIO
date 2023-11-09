#include "ycsb_test.h"

#include <vector>
#include <deque>

#include "nexio_test.h"
#include "memlayer/MemTier.h"
#include "uniform_generator.h"




void ycsb_update_heavy(uint64_t modified_data_num) {
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
               for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j){
                    value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point+1 >> (8 * j)) & 0xFF);
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


void ycsb_read_heavy(uint64_t modified_data_num) {
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
               for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j){
                    value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point+1 >> (8 * j)) & 0xFF);
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


void ycsb_read_only(uint64_t modified_data_num) {
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




void ycsb_read_latest(size_t operationsCount, std::deque<std::vector<char>>& recentKeys) 
{ 
     UniformGenerator keyGenerator(0, operationsCount);
     uint64_t data_point = keyGenerator.next();
     for (size_t i = 0; i < operationsCount/2; ++i) {
          data_point= keyGenerator.next();
          for (size_t j = 0; j < sizeof(uint64_t) && j < KEY_SIZE; ++j){
               key_buffer[KEY_SIZE - 1 - j] = static_cast<char>((data_point >> (8 * j)) & 0xFF);
          }
          for (size_t j = 0; j < sizeof(uint64_t) && j < VAL_SIZE; ++j){
               value_buffer[KEY_SIZE - error_bound - 1 - j] = static_cast<char>((data_point+1 >> (8 * j)) & 0xFF);
          }
        if (rand() % 100 < 50) {
            // 执行插入操作
            InsertNode(key_buffer, value_buffer);
            // 将新键添加到最近使用的键队列中
            recentKeys.push_back(std::vector<char>(key_buffer, key_buffer + KEY_SIZE));
          } else {
            // 执行更新操作
            Update(key_buffer, value_buffer);
            // 这里我们假设更新的是最近的键之一
            // 如果您有不同的更新策略，请相应调整
        }
        // 保持最近键队列的大小
        if (recentKeys.size() > 100) { // 最近键队列的大小限制为100
            recentKeys.pop_front();
        }
    }



     for (size_t i = 0; i < operationsCount; ++i) {
          if ((rand() % 100) < 95) {
               if (!recentKeys.empty()) 
               {
                    size_t randomIndex = rand() % recentKeys.size();
                    std::vector<char> keyToRead = recentKeys[randomIndex];
                    memcpy(key_buffer, keyToRead.data(), KEY_SIZE);
                    uint64_t value = Search(key_buffer); 
               }
          }else {
             
               InsertNode(key_buffer, value_buffer);
               recentKeys.push_back(std::vector<char>(key_buffer, key_buffer + KEY_SIZE));
               if (recentKeys.size() > 100) {
                    recentKeys.pop_front();
               }
        }
    }
}
