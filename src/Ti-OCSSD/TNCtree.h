/**
 * @date   28/9/2022
 * @author zz.wang
 * @details
 *         This file is a port of 2-tier none-coupling tree and used to
 *         initialize asynchornized or synchornized write thread.  
 * 
 * IDENTIFICATION:
 *         src/TNCTree/TNCtree.h
 */

#ifndef EXPERIMENT1_TNCTREE_H
#define EXPERIMENT1_TNCTREE_H

#include <unistd.h>
#include <sys/types.h>

extern int reads;
extern int writes;
extern int erases;

extern volatile int msqid;

/* ================= 2-Tier None-Coupling module ==================== */

int sub_process_init(void);


void TiOCSInit(void);

#endif //EXPERIMENT1_FRONTMAIN_H
