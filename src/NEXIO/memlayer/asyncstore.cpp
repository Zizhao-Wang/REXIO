#include "asyncstore.h"
#include <unistd.h>
#include "disklayer/write_buffer.h"
#include "include/buffer.h"


int MAX_write_buffer_TASKS = -1;




/**
 *  ================= Asynchronous write module====================  
 **/

/**
 * @brief Predicts the next available offset for writing.
 *
 * This function calculates and updates the next suitable offset for data placement 
 * based on the current state and the SSD geometry. It serves as a mechanism to 
 * ensure that data is laid out efficiently on the SSD.
 *
 * @return Predicted offset for the next data write.
 */
uint32_t predict_next_offset()
{
    // offset5 += page_pointer % geometry.clba == 0 ? 0x1000000 : 0x00000000;

    offset5 = offset5 & 0xFF000FFF;
    // offset5 += ((page_pointer % 4096/4) << 12);

    offset5 = offset5 & 0xFFFFF000;
    // indexs = 0;

    return offset5;  // Assuming you meant to return the updated offset
}

/**
 * @brief Asynchronously writes key-value pairs.
 *
 * This function writes the provided key-value pairs into the write buffer in 
 * an asynchronous manner. If the buffer is full, a signal is sent to indicate 
 * that the buffer is ready for I/O operations.
 *
 * @param hashkey   The hash key to write.
 * @param hashvalue The hash value corresponding to the key.
 * @return Predicted offset where the key-value pair would be written on the disk.
 */
uint32_t async_write(const char* hashkey, const char* hashvalue)
{
    uint32_t entry_offset_within_disk = predict_next_offset();

    // Lock buffer access to ensure thread-safety
    pthread_mutex_lock(&write_buffer_mutex);

    // Copy key-value pair to buffer
    memcpy(combined_kv_buffer[current_buffer_position].key, hashkey, KEY_SIZE);
    memcpy(combined_kv_buffer[current_buffer_position].val, hashvalue, VAL_SIZE);
    current_buffer_position++;

    // Check if buffer is full and signal accordingly
    if (current_buffer_position >= MAX_write_buffer_TASKS) 
    {
        printf("\033[0;33m[INFO]\033[0m Write buffer is full. Signaling for I/O operations.\n");
        current_buffer_position = 0;  
        pthread_cond_signal(&buffer_full_cond_var);  
    }

    pthread_mutex_unlock(&write_buffer_mutex);

    return entry_offset_within_disk;
}

