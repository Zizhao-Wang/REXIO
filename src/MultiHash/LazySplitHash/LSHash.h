/**
 * @date:   21/9/2022
 * @author: zz.wang
 * @email:  zz.wang@siat.ac.cn
 * @details:
 *          This file shows how to construct a Lazy-split Hash.
 *
 * IDENTIFICATION:
 *          src/LazySplit-Hash/LSHash.h
 **/

#ifndef EXPERIMENT1_LAZYSPLITHASH_H
#define EXPERIMENT1_LAZYSPLITHASH_H

#include <iostream>
#include <vector>
#include "../Bucket.h"
#include "../../Auxizilary/GlobalVariable.h"


#define DEFAULT_BUCKETMAXSIZE 100
#define DEFAULT_TABLEBASESIZE 100

class LSbucket : public bucket
{
protected:
	std::vector<SEntry> BucketEntries;

public:
	LSbucket(uint16_t maxsize);

	int Insert(SKey key, SValue value);
	SValue Retrieval(SKey key);
    int Delete(SKey key);
    int Update(SKey key, SValue value);
    

    bool IsFull(void) const;
    bool IsEmpty(void) const;
	

    void AllClear(void) ;
    int PageWrite() ;

};


class LSHash
{

private:
	std::vector<LSbucket> bucketList;
	uint64_t Allsize;

public:
	LSHash(uint16_t , uint16_t);
	int Insert(SKey key, SValue value);
	SValue Retrieval(SKey key);
    int Delete(SKey key);
    int Update(SKey key, SValue value);

	double IFCompute();
	
};


void LSHashPort();




#endif //EXPERIMENT1_LAZYSPLITHASH_H