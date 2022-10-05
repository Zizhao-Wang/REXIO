/**
 * @date:   4/8/2022
 * @author: zz.wang
 * @details:
 *          This file contains all fuction declaration and definition of class used in "Lhash.cpp". 
 *
 * IDENTIFICATION:
 *          src/MultipleHash/Bucket.h
 **/

#ifndef EXPERIMENT1_LINEARHASH_NODE_H
#define EXPERIMENT1_LINEARHASH_NODE_H

#include "../../Auxizilary/GlobalVariable.h"


typedef struct LHEntry
{
	SKey   key;
	SValue val;

	bool operator == (const LHEntry& other) const 
    {
        return key == other.key;
    }
    bool operator<(const LHEntry& other) const 
    {
        return key < other.key;
    }
}LHEntry;


#endif