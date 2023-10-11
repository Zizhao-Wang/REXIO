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
#include "../BlockManage/BlockInfo.h"
// #include "../../Auxizilary/pre_definition.h"
#include "../../Auxizilary/SysOutput.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../Backend/backend_variables.h"
using namespace std;


/* 
 * Global variables for SSD write definition. 
 */
extern size_t writepointer;
extern size_t sectorpointer;
extern size_t blockpointer;


extern bool flag;



/* Initialize the global block pointer */
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

int erasechunk(uint64_t chunkno);

int erasepage(size_t pageno);

/* function is used to update pointers.  */
int PointerRenew(size_t sectors);



/* out-of-place update */
int BlockUpdate(size_t pageno, uint64_t value, uint64_t Cursize);

int PageUpdate(size_t pageno, uint64_t value, uint64_t Cursize);



/**
 *  Function declartion for writing data into one or more pages:
 **/
int WriteintoSector(nvm_bp *bp);

int ReadtoSector();

uint_32 SSD_write(uint64_t value);

// write pages into SSD
uint64_t SSD_write2(uint64_t values);

// Insert signle value into physicals.
uint64_t SingleValueWrite(uint64_t value, uint64_t pageno,uint64_t Cursize);

// Insert page-level key-value pairs into physicals for linear hashing.

int MultiPageWrite(std::vector<entry_t> entries, std::vector<uint64_t> pagelist);




/**
 * Function declartion for reading from one or more pages:
 *   
 **/

int RunDataErase(std::vector<uint64_t> pageno);

/* Some other auxizilary functions. */
uint64_t GetPagesize(void);

uint64_t CalculatePageCapacity(size_t KVsize);

#endif //EXPERIMENT1_WRITEDISK_H