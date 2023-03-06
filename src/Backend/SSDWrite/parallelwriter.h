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



/**
 * @brief 
 * @param Entries 
 * @param pageno 
 * @return uint64_t 
 */


uint64_t async_write_channel(std::vector<entry_t> Entries, uint64_t pageno);




#endif  //TiOCS_BACKEND_PARALLELWRITER_H