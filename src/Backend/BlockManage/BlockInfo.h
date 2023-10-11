/**
 * @date 2022.6.16 16:48
 * @author zz.wang
 * @details
 *          BlockInfo.h implemented the abstract of block and page, including some main structs:
 *              1. BlockInformation
 *              2.
 *
 *
 *
 *         IDENTIFICATION
 *   Experiment1/Backend/BlockManage/BlockInfo.h
 */


#ifndef EXPERIMENT1_BLOCKINFO_H
#define EXPERIMENT1_BLOCKINFO_H

#include "../../Auxizilary/GlobalVariable.h"


typedef struct PageInfo
{
    uint_32 pageid;
    uint_32 size;

}PageInfo;

typedef struct BlockInfo
{

    uint_32 blockid;
    uint_32 size;

}BlockInfo;


int vblk_ex01_ewr(struct nvm_bp *bp);

int SSD_init();



#endif //EXPERIMENT1_BLOCKINFO_H
