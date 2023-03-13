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




#endif //TIOCS_BACKEND_VARIABLES_H