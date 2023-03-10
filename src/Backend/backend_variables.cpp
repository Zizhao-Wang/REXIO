#include "backend_variables.h"



/* global variables for recording usage of physical pages  */
int writes = 0;
int reads = 0;
int erases = 0;


/* chunk_usage is a hash table that recording the write pointer each block(chunk) for exploiting the parallelism of OCSSD   */
std::unordered_map<uint64_t,std::vector<uint64_t>> chunk_usage;