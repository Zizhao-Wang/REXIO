/**
 * @date:   26/9/2022
 * @author: zz.wang
 * @email:  zz.wang@siat.ac.cn
 * @details:
 *          This file shows how to construct a generalized bucket for xxx-hash.
 *
 * IDENTIFICATION:
 *          src/MultipleHash/Bucket.h
 **/

#ifndef EXPERIMENT1_BUCKET_H
#define EXPERIMENT1_Multi_BUCKET_H

#include <iostream>
#include "../Auxizilary/GlobalVariable.h"

class bucket 
{

protected:
	uint16_t CurrentSize, MaxSize; //page number and current size 
	uint64_t PageNo;

public:
    bucket(uint16_t maxsize):MaxSize(maxsize){};
   /**
	* As a rule, a complete index structure should at least include the methods for CURD and crash recovery.
	* Create, update, retrieval, delete and crash recovery.
	**/
    int Insert(SKey key, SValue value);
    int Delete(SKey key);
    int Update(SKey key, SValue value);
    SValue Retrieval(SKey key);

    bool IsFull(void) const;
    bool IsEmpty(void) const;

    void AllClear(void);
    int  PageWrite();

};


#endif //EXPERIMENT1_LAZYSPLITHASH_H