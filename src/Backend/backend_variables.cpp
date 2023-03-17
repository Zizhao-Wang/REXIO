#include "backend_variables.h"
#include "../global_variables/global_variables.h"


/* global variables for recording usage of physical pages  */
int writes = 0;
int reads = 0;
int erases = 0;


/* chunk_usage is a hash table that recording the write pointer each block(chunk) for exploiting the parallelism of OCSSD   */
std::unordered_map<uint64_t,uint64_t> chunk_usage;



/* global variables for nvm_cmd_read/write/erase */
size_t ws_min = 4;  // set 4 by default


/* global variables for asynchronous writing/read */
int max_os_threads = 0;  // set 8 by default



/* initialize chunk write pointer  */
size_t *chunk_write_pointer = nullptr;

size_t *lun_current_pointer = nullptr;



