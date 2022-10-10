/**
 * @date:   10/10/2022
 * @author: zz.wang
 * @details:
 *          This file shows how to construct a Lazy-split Hash.
 *
 * IDENTIFICATION:
 *          src/LazySplit-Hash/LSHash.h
 **/

#ifndef EXPERIMENT1_LAZYSPLITHASH_LSNODE_H
#define EXPERIMENT1_LAZYSPLITHASH_LSNODE_H


#include <iostream>

typedef struct LSEntry
{
	SKey key1;
	SValue val;
	uint8_t flag;
	
}LSEntry;


#endif