/**
 * @date    2/10/2022 
 * @author  zz.wang
 * @details
 *          This file implemented the data structure of the node in Hash Table!
 *
 * IDENTIFICATION:
 *          src/TNCTree/StoreTier/asnycstore.h
 */

#ifndef EXPERIMENT1_TNCTREE_ASYNCSTORE_H
#define EXPERIMENT1_TNCTREE_ASYNCSTORE_H


#include <stdint.h>
#include <iostream>
#include "../../Auxizilary/GlobalVariable.h"


int CreateMultiProcess();

uint32_t AsyncWrite(SKey key, SValue value); 

SValue   AsyncRead(uint32_t offset);


#endif

