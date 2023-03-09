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
#include "../../Ti-OCSSD/MemoryTier/Node.h"
#include "../../Auxizilary/VariablesTypes.h"
#include "../../Auxizilary/SysOutput.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../Ti-OCSSD/StoreTier/syncstore.h"
#include "../../MultiHash/LinearHash/node.h"
#include "../../MultiHash/LinearHash/LiHash.h"
#include "../../MultiHash/ExtendibleHash/ExNode.h"
#include "../../MultiHash/ExtendibleHash/ExHash.h"

/*  Global variables for SSD write definition.  */

extern std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkLog;  
extern std::unordered_map<uint64_t,std::vector<uint64_t>> ChunkData;
extern std::unordered_map<uint64_t,bool[1024]> GPT;
extern PageType DataPagePointer;

/**
 * ============= TNC-tree methods module ===============
 *  Function declartion for writing data into one or more pages:
 **/
int InfoRenew(size_t scale, PageType LogPointer, bool flag);

int SinglePageWrite(); // Insert page-level key-value pairs into physicals for TNC-tree.

int PageLogWrite(uint64_t BlockId);

/**
 * ============= Linear Hash module ===============
 *  Function declartion for writing data into one or more pages:
 **/
int BucketWrite(std::vector<LHEntry> entries, uint64_t pageno);

int PageUpdatePageUpdate(PageType pageno, std::vector<LHEntry> entries);

PageType SingleBucketWrite(std::vector<LHEntry> entries, uint64_t pageno);

/**
 * ============= Extendible Hash module ===============
 *  Function declartion for writing data into one or more pages:
 **/
int BucketWrite(std::vector<ExEntry> entries, uint64_t pageno);

int PageUpdatePageUpdate(PageType pageno, std::vector<ExEntry> entries);

PageType SingleBucketWrite(std::vector<ExEntry> entries, uint64_t pageno);


/**
 * ============= Lazy-split Hash module ===============
 *  Function declartion for writing data into one or more pages:
 **/
int LSBucketWrite(std::vector<LHEntry> entries, uint64_t pageno);

/**
 * ============= LSM-tree module ===============
 *  Function declartion for writing data into one or more pages:
 **/
PageType SinglePageWrite(std::vector<entry_t> Entries, uint64_t pageno);



void GPTDisplay(void);


#endif //EXPERIMENT1_WRITEDISK_H