/**
 * @date:   28/9/2022
 * @author: zz.wang
 * @details:
 *          This file shows how to construct a Eager-Split Hash.
 *
 * IDENTIFICATION:
 *          src/MultiHash/EagerSplitHash/ESHash.h
 **/

#ifndef EXPERIMENT1_EAGERSPLITHASH_ESHASH_H
#define EXPERIMENT1_EAGERSPLITHASH_ESHASH_H

#include <iostream>
#include <vector>
#include "../Bucket.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../Auxizilary/SysOutput.h"


#define DEFAULT_BUCKETMAXSIZE 100
#define DEFAULT_TABLEBASESIZE 100
#define DEFAULT_IFTHRESHOLD   0.9

class LSbucket : public bucket
{
protected:
	std::vector<SEntry> BucketEntries;

public:
	LSbucket(uint16_t maxsize);

	int Insert(SKey key, SValue value);
	SEntry Retrieval(SKey key);
    int Delete(SKey key);
    int Update(SKey key, SValue value);
    
    bool IsFull(void) const;
    bool IsEmpty(void) const;

    void AllClear(void);
	uint16_t GetMaxSize() const;
	std::vector<SEntry> Getdata() const;
    int PageWrite();

};


class LSHash
{

private:
	std::vector<LSbucket> bucketList;
	uint64_t Allsize;
	uint16_t bmaxsize;
	double  IFthreshold;

public:
	LSHash(uint16_t , uint16_t, double ifth);

	SKey BitHashfunc(SKey, uint8_t);
	uint8_t  GetBits(SKey);
	void Split(size_t BucketNo);

	double IFCompute();
	int Insert(SKey key, SValue value);
	SEntry Retrieval(SKey key);
    int Delete(SKey key);
    int Update(SKey key, SValue value);
	
};


void LSHashPort();




#endif //EXPERIMENT1_LAZYSPLITHASH_H