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


#include "../BlockManage/BlockInfo.h"
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <liblightnvm.h>
using namespace std;


/* 
 * Global variables for SSD write definition. 
 */
extern uint_32 blockoffset;
extern size_t writepointer;
extern size_t sectorpointer ;
extern size_t blockpointer ;
extern int indexs;
extern struct nvm_bp* bp;
extern bool flag;
extern uint64_t Pagedata[2050];
// Initialize the global block pointer
extern std::unordered_map<uint64_t,uint64_t> chunkusage;

/* 
 * Erase functions declartion.
 */
// Erase a sprcific chunck.
int erasechunk(uint64_t sectorno);

// Erase a specific page.
int erasepage(uint64_t pageno);


/* function is used to update pointers.  */
int PointerRenew(size_t sectors);



/* 
 * Write functions declartion. 
 */
int WriteintoSector(nvm_bp *bp);

int ReadtoSector();

uint_32 SSD_write(uint64_t value);

// write pages into SSD
uint64_t SSD_write2(uint64_t values);

//Signle value insert into physicals.
uint64_t SVwrite(uint64_t value, uint64_t pageno,uint64_t Cursize);



/* 
 * Read functions declartion. 
 */
int ReadfromPage(uint64_t pageno);

#endif //EXPERIMENT1_WRITEDISK_H
