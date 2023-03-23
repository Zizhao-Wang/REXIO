/**
 * @date   6/3/2022 
 * @author zz.wang
 * @details
 *         Parallelism is the main feature of OpenChannel Solid State Drive (OCSSD).
 * 
 * IDENTIFICATION:
 *   	   TiOCS/src/Backend/SSDWrite/parallelwriter.h
 */

#ifndef TiOCS_BACKEND_PARALLELWRITER_H
#define TiOCS_BACKEND_PARALLELWRITER_H
#include <liblightnvm.h>
#include <vector>
#include "../../Auxizilary/pre_definition.h"





/**
 * @brief 
 * @param Entries 
 * @param pageno 
 * @return uint64_t 
 */


uint64_t async_write_channel(std::vector<entry_t> Entries, uint64_t pageno);


/* parallel read/write function */
void* parallel_write_into_pu(void *args);

void* parallel_read_from_pu(void *args);




void* parallel_coordinator(std::vector<entry_t> run_data, uint64_t num_lun, int mode, void* read_param);


#endif  //TiOCS_BACKEND_PARALLELWRITER_H