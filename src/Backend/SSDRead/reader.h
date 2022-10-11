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
#include "../../Auxizilary/VariablesTypes.h"
#include "../../MultiHash/LazySplitHash/LSNode.h"
/**
 * Function declartion for reading from one or more pages:
 * Two-tier none-coupling tree  
 **/
int SinglePageRead(PageType pageno);

TNCEntry* TNCEntryRead(PageType PageId);


/**
 * ============= Linear Hash module ===============
 *  Function declartion for writing data into one or more pages:
 **/
std::vector<LHEntry> PageRead(PageType PageNum);

int PageReadTest(PageType PageNum);

/**
 * ============= LSM-tree module ===============
 *  Function declartion for writing data into one or more pages:
 **/
int PageDataRead(PageType pageno);

std::vector<entry_t> RunReadFromPage(PageType PageNum);

/**
 * ============= Lazy-split hashing module ===============
 *  Function declartion for writing data into one or more pages:
 **/
std::vector<LSEntry> LSBucketFromPage(PageType PageNum, int size=0);

#endif //EXPERIMENT1_WRITEDISK_H