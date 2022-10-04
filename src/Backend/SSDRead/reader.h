/**
 * @date   29/9/2022 
 * @author zz.wang
 * @details
 *         There are two main processes in the backend:
 * 		   Writer process and reader process.
 * 
 * IDENTIFICATION:
 *   	   src/Backend/SSDRead/reader.h
 */

#ifndef EXPERIMENT1_BACKEND_READER_H
#define EXPERIMENT1_BACKEND_READER_H


#include <stdint.h>
#include <vector>
#include <liblightnvm.h>
#include "../../Auxizilary/SysOutput.h"
#include "../../TNCTree/StoreTier/syncstore.h"
#include "../../TNCTree/StoreTier/asyncstore.h"
#include "../../TNCTree/utility/types.h"
#include "../../MultiHash/LinearHash/node.h"
/**
 * Function declartion for reading from one or more pages:
 * Two-tier none-coupling tree  
 **/
int SinglePageRead(PageType pageno);

TNCEntry* TNCEntryRead(PageType PageId);


/**
 * Function declartion for reading from one or more pages:
 * Linear Hash   
 **/
std::vector<LHEntry> PageRead(PageType PageNum);


#endif //EXPERIMENT1_WRITEDISK_H