/**
 * @date:   4/8/2022
 * @author: zz.wang
 * @details:
 *          This file contains all fuction declaration and definition of class used in "Lhash.cpp". 
 *
 * IDENTIFICATION:
 *          src/MultiHash/ExtendibleHash/ExNode.h
 **/

#ifndef EXPERIMENT1_EXTENDIBLEHASH_EXNODE_H
#define EXPERIMENT1_EXTENDIBLEHASH_EXNODE_H

#include "../../Auxizilary/GlobalVariable.h"
#include "../../Backend/backend_variables.h"

typedef struct ExEntry
{
	SKey   key;
	SValue val;

	bool operator == (const ExEntry& other) const 
    {
        return key == other.key;
    }
    
    bool operator<(const ExEntry& other) const 
    {
        return key < other.key;
    }

}ExEntry;

#endif