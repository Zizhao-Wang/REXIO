#include "write_buffer.h"
#include "include/buffer.h"

// ==========================
// Offset and buffer related 
// ==========================
uint32_t offset5 = 0;
uint32_t offset2 = 0; 
size_t buffer_capacity;

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
char *log_data_buffer = nullptr;
size_t log_block_id_allocator = 0;
std::unordered_map<uint64_t, int> block_type_tracker;


// ======================
// Combined key-value store
// ======================
key_value_entry *combined_kv_buffer;
page_num_type combined_kv_page_num = 0;

// ==========================
// Synchronization primitives
// ==========================
pthread_mutex_t write_buffer_mutex;
pthread_cond_t buffer_full_cond_var;
int current_buffer_position = 0;


// ==========
// Temp storage
// ==========
char *temp = nullptr;



void initialize_write_buffer_variables() 
{
    // Initialize write buffer mutex
    if (pthread_mutex_init(&write_buffer_mutex, NULL) != 0) {
        printf("\033[1;31mError: Failed to initialize write buffer mutex.\033[0m\n");
        return;
    }
    printf("\033[1;32mSuccessfully initialized write buffer mutex.\033[0m\n");

    // Initialize buffer full condition variable
    if (pthread_cond_init(&buffer_full_cond_var, NULL) != 0) {
        printf("\033[1;31mError: Failed to initialize buffer full condition variable.\033[0m\n");
        return;
    }
    printf("\033[1;32mSuccessfully initialized buffer full condition variable.\033[0m\n");
}

void combined_kv_buffer_init()
{
    combined_kv_buffer = (key_value_entry*)spdk_dma_malloc(sizeof(key_value_entry) * buffer_capacity, 0x1000, NULL);
    
    if (combined_kv_buffer == NULL) {
        printf("\033[1;31mError: Failed to allocate memory for combined KV buffer.\033[0m\n");
        return;
    }
    printf("\033[1;32mSuccessfully allocated memory for combined KV buffer.\033[0m\n");
}

void combined_kv_buffer_cleanup() 
{
    spdk_dma_free(combined_kv_buffer);
    printf("\033[1;32mSuccessfully deallocated memory for combined KV buffer.\033[0m\n");
}



void kv_buffer_init() 
{
    size_t key_buffer_size = 0;
    size_t value_buffer_size = 0;
    size_t log_buffer_size = 0;

    key_separated_buffer = (char*)spdk_dma_malloc(key_buffer_size, 0x1000, NULL);
    if (key_separated_buffer == NULL) 
    {
        printf("\033[1;31mError: Failed to allocate memory for KeySeparatedBuffer.\033[0m\n");
    }

    value_separated_buffer = (char*)spdk_dma_malloc(value_buffer_size, 0x1000, NULL);
    if (value_separated_buffer == NULL) 
    {
        printf("\033[1;31mError: Failed to allocate memory for ValueSeparatedBuffer.\033[0m\n");
    }

    log_data_buffer = (char *)spdk_dma_malloc(log_buffer_size, 0x1000, NULL);
    if (log_data_buffer  == NULL)
    {
        printf("\033[1;31mError: Failed to allocate memory for LogDataBuffer!\033[0m\n");
        exit(-1);
    }

    if(value_block_id == UINT64_MAX)
    {
        value_block_id = block_id_allocator++;
        block_type_tracker[value_block_id] = VALUE_BLOCK;
        offset5 = (offset5 & 0x00FFFFFF) | (value_block_id << 24);
    }

    if(key_block_id == UINT64_MAX)
    {
        key_block_id = block_id_allocator++;
        block_type_tracker[key_block_id] = KEY_BLOCK;
        offset2 = (offset2 & 0x00FFFFFF) | (key_block_id << 24);
    }

    temp = (char *)spdk_dma_malloc(sizeof(uint32_t), 0x1000, NULL);

}


void kv_buffer_cleanup() 
{
    spdk_dma_free(key_separated_buffer);
    spdk_dma_free(value_separated_buffer);
    spdk_dma_free(log_data_buffer);
}




