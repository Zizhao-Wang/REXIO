#include "write_buffer.h"
#include "include/buffer.h"
#include "include/spdk_env_init.h"
#include "utility/types.h"

// ==========================
// Offset and buffer related 
// ==========================
uint64_t offset5 = 0;
uint64_t offset2 = 0; 
uint64_t num_data_page = 0;
write_io_controller my_controller;
// ========================
// Separated key-value store
// ========================
size_t key_position_in_buffer = 0;
char *key_separated_buffer = nullptr;

size_t value_position_in_buffer = 0;
char *value_separated_buffer = nullptr;

size_t key_block_id = UINT64_MAX;
size_t value_block_id =  UINT64_MAX;

size_t block_id_allocator = 0;

// ==========
// Log buffer
// ==========
size_t log_block_id_allocator = 0;
std::unordered_map<uint64_t, int> block_type_tracker;
std::unordered_map<uint64_t, std::vector<char>> log_buffer;

// ======================
// Combined key-value store
// ======================
char* combined_kv_buffer;
page_num_type combined_kv_page_num = 0;

// ==========================
// Synchronization primitives
// ==========================
pthread_mutex_t write_buffer_mutex;
pthread_cond_t buffer_full_cond_var;
int current_buffer_position;


// ==========
// Temp storage
// ==========
char *temp = nullptr;



uint64_t write_buffer_id = 0;


void initialize_write_buffer_variables() 
{
    // Initialize write buffer mutex
    if (pthread_mutex_init(&write_buffer_mutex, NULL) != 0) {
        printf("\033[0;31m[ERROR]\033[0m Failed to initialize write buffer mutex.\n");
        return;
    }
    printf("\033[1;32m[SUCCESS]\033[0m Successfully initialized write buffer mutex.\n");

    // Initialize buffer full condition variable
    if (pthread_cond_init(&buffer_full_cond_var, NULL) != 0) {
        printf("\033[0;31m[ERROR]\033[0m Failed to initialize buffer full condition variable.\n");
        return;
    }
    my_controller.buffer_capacity = (SPDK_LBAs_IN_NEXIO_WRITE_BUFFER*device_info->ns_info_array[0].lba_size) / sizeof(KEY_SIZE+FLAGS_value_size);
    // printf("%lu\n", my_controller.buffer_capacity);
    my_controller.nexio_write_uint =  SPDK_LBAs_IN_NEXIO_WRITE_BUFFER; //default: The nexio block contains 2 lbas
    my_controller.nexio_lba_uint = SPDK_LBAs_IN_NEXIO_LBA;
    my_controller.nexio_log_unit = SPDK_LBAs_IN_NEXIO_LOG_BUFFER;
    my_controller.write_buffer_size = SPDK_LBAs_IN_NEXIO_WRITE_BUFFER*device_info->ns_info_array[0].lba_size;
    my_controller.nexio_log_buffer_size = SPDK_LBAs_IN_NEXIO_LOG_BUFFER*device_info->ns_info_array[0].lba_size;
    my_controller.current_write_lba_num = 0;

    my_controller.nexio_data_page_num_in_block = num_data_page /my_controller.nexio_write_uint;
    my_controller.nexio_log_page_num_in_block = (my_controller.nexio_lba_uint - num_data_page)/my_controller.nexio_write_uint;
    current_buffer_position = 0;
    printf("\033[1;32m[SUCCESS]\033[0m Successfully initialized buffer full condition variable.\n");
}

void combined_kv_buffer_init()
{
    initialize_write_buffer_variables();
    combined_kv_buffer = (char*)spdk_dma_malloc((KEY_SIZE+value_size)*my_controller.buffer_capacity, 0x1000, NULL);
    if (combined_kv_buffer == NULL) {
        printf("\033[0;31m[ERROR]\033[0m Failed to allocate memory for combined KV buffer.\n");
        return;
    }
    printf("\033[1;32m[SUCCESS]\033[0m Successfully allocated memory for combined KV buffer.\n");
}

void combined_kv_buffer_cleanup() 
{
    spdk_dma_free(combined_kv_buffer);
    fprintf(stdout, "\033[1;32m[SUCCESS]\033[0m Successfully deallocated memory for combined KV buffer.\n");
}




void kv_buffer_init() 
{

    size_t log_buffer_size = 0;

    key_separated_buffer = (char*)spdk_dma_malloc(my_controller.write_buffer_size, 0x1000, NULL);
    if (key_separated_buffer == NULL) 
    {
        printf("\033[1;31mError: Failed to allocate memory for KeySeparatedBuffer.\033[0m\n");
    }

    value_separated_buffer = (char*)spdk_dma_malloc(my_controller.write_buffer_size, 0x1000, NULL);
    if (value_separated_buffer == NULL){
        printf("\033[1;31mError: Failed to allocate memory for ValueSeparatedBuffer.\033[0m\n");
    }

    

    if(value_block_id == UINT64_MAX)
    {
        value_block_id = block_id_allocator++;
        block_type_tracker[value_block_id] = VALUE_BLOCK;
        offset5 = (offset5 & 0x0000000000FFFFFF) | (value_block_id << 24);
        write_buffer_id = 0;
    }
    my_controller.current_write_lba_num = value_block_id * my_controller.nexio_lba_uint;
    printf("First init::my_controller.current_write_lba_num:%lu....\n",my_controller.current_write_lba_num);

    if(key_block_id == UINT64_MAX)
    {
        key_block_id = block_id_allocator++;
        block_type_tracker[key_block_id] = KEY_BLOCK;
        offset2 = (offset2 & 0x0000000000FFFFFF) | (key_block_id << 24);
    }
    my_controller.key_write_num = key_block_id * my_controller.nexio_lba_uint;

    temp = (char *)spdk_dma_malloc(sizeof(uint32_t), 0x1000, NULL);

}


void kv_buffer_cleanup() 
{
    spdk_dma_free(key_separated_buffer);
    spdk_dma_free(value_separated_buffer);
}
