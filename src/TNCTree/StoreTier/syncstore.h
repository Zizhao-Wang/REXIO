/**
 * @date    2/10/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/StoreTier/asnycstore.h
 */

#ifndef EXPERIMENT1_TNCTREE_SYNCSTORE_H
#define EXPERIMENT1_TNCTREE_SYNCSTORE_H

#include <stdint.h>
#include <iostream>
#include "../../Auxizilary/GlobalVariable.h"

/* Global variable declaration! */
extern uint32_t offset;

/**
 *  ================= Synchronous write module====================  
 **/
uint32_t SyncWrite(SKey key1, SValue value);

/**
 *  ================= Synchronous delete module====================  
 **/
int      SyncDelete(uint32_t offset);

/**
 *  ================= Synchronous read module====================  
 **/
SValue   syncRead(uint32_t offset);

#endif

