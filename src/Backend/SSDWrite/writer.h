/**
 * @date   29/9/2022 
 * @author zz.wang
 * @details
 *         There are two main processes in the backend. More details about them are as follows:
 * 
 * IDENTIFICATION:
 *   	   src/Backend/SSDWrite/writer.h
 */

#ifndef EXPERIMENT1_BACKEND_WRITER_H
#define EXPERIMENT1_BACKEND_WRITER_H


#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <liblightnvm.h>
#include "../../TNCTree/MemoryTier/Node.h"
#include "../../Auxizilary/VariablesTypes.h"
#include "../../Auxizilary/SysOutput.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../TNCTree/StoreTier/syncstore.h"

/*  Global variables for SSD write definition.  */

extern std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkLog;  

extern std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkData;

extern PageType DataPagePointer;

/**
 * ============= writer methods module ===============
 *  Function declartion for writing data into one or more pages:
 **/
int InfoRenew(size_t scale, PageType LogPointer, bool flag);

int SinglePageWrite(); // Insert page-level key-value pairs into physicals for TNC-tree.

int PageLogWrite(uint64_t BlockId);


#endif //EXPERIMENT1_WRITEDISK_H