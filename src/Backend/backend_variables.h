/**
 * @date   9/3/2022 
 * @author zz.wang
 * @details
 *         This file is used to declare variables for backend threads(write/read/erase).
 * 
 * IDENTIFICATION:
 *   	   TiOCS/src/Backend/backend_variables.h
 */

#ifndef TIOCS_BACKEND_VARIABLES_H
#define TIOCS_BACKEND_VARIABLES_H

#include <unordered_map>
#include <vector>


extern int writes;
extern int reads;
extern int erases;

extern size_t ws_min;


extern std::unordered_map<uint64_t,uint64_t> chunk_usage;


/* global variables for SSD geometery */
typedef uint64_t SKey;
typedef uint64_t SValue;

#define DELETEVALUE UINT64_MAX

typedef struct SEntry
{
	SKey key1;
	SValue val;
}SEntry;


/**
 *  
 **/
typedef uint64_t PageType;
typedef uint64_t BlockType;




extern int max_os_threads;

struct thread_param
{
	char* buffer;
	PageType page_num;
	size_t size;
};




extern size_t *chunk_write_pointer;

extern size_t *lun_current_pointer;

extern size_t *channel_current_pointer;


/**
 *  signal handler for SIGINT 
 **/

#define PAOCS_READ_MODE  0X0001
#define PAOCS_WRITE_MODE 0X0002
#define PAOCS_ERASE_MODE 0X0003

typedef struct coordinator_param
{
	size_t start_page, end_page;
	size_t size;
}coordinator_param;


#endif //TIOCS_BACKEND_VARIABLES_H