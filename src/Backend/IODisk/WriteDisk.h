/**
 * @date 2022.5.15 11:22
 * @author zz.wang
 * @details
 *          There are two main processes in the backend. More details about them are as follows::
 *
 *           Backend write process:
 *           Backend read process:
 *
 *         IDENTIFICATION
 *   Experiment1/Backend/BackendMain.h
 */

#ifndef EXPERIMENT1_WRITEDISK_H
#define EXPERIMENT1_WRITEDISK_H



#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <liblightnvm.h>
#include "../BlockManage/BlockInfo.h"
#include "../../Auxizilary/VariablesTypes.h"
#include "../../Auxizilary/SysOutput.h"
#include "../../Auxizilary/GlobalVariable.h"
using namespace std;


/* 
 * Global variables for SSD write definition. 
 */
extern uint_32 blockoffset;
extern size_t writepointer;
extern size_t sectorpointer;
extern size_t blockpointer;
extern int indexs;
extern struct nvm_bp* bp;
extern bool flag;
extern uint64_t Pagedata[2050];
// Initialize the global block pointer
extern std::unordered_map<uint64_t,uint64_t> chunkusage;



/* 
 * other compensate functions 
 */
int CompenstaeFun(uint64_t chunkno);

/* 
 * Erase functions declartion.
 */
// Erase a sprcific chunck.
int erasechunk(size_t pageno, uint64_t chunkno);

int erasepage(size_t pageno);

/* function is used to update pointers.  */
int PointerRenew(size_t sectors);



/* out-of-place update */
int BlockUpdate(size_t pageno, uint64_t value, uint64_t Cursize);

int PageUpdate(size_t pageno, uint64_t value, uint64_t Cursize);



/* 
 * Write functions declartion. 
 */
int WriteintoSector(nvm_bp *bp);

int ReadtoSector();

uint_32 SSD_write(uint64_t value);

// write pages into SSD
uint64_t SSD_write2(uint64_t values);

// Insert signle value into physicals.
uint64_t SingleValueWrite(uint64_t value, uint64_t pageno,uint64_t Cursize);

// Insert page-level key-value pairs into physicals for linear hashing.
uint64_t SinglePageWrite(std::vector<entry_t> Entries, uint64_t pageno);

int MultiPageWrite(std::vector<entry_t> entries, std::vector<uint64_t> pagelist);


/* 
 * Read functions declartion. 
 */
int PageDataRead(uint64_t pageno);

entry_t* RunReadFromPage(uint64_t PageNum, size_t Runsize);

int RunDataErase(std::vector<uint64_t> pageno);

/* Some other auxizilary functions. */
uint64_t GetPagesize(void);

uint64_t CalculatePageCapacity(size_t KVsize);

#endif //EXPERIMENT1_WRITEDISK_H