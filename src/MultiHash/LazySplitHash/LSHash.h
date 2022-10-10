/**
 * @date:   21/9/2022
 * @author: zz.wang
 * @details:
 *          This file shows how to construct a Lazy-split Hash.
 *
 * IDENTIFICATION:
 *          src/LazySplit-Hash/LSHash.h
 **/

#ifndef EXPERIMENT1_LAZYSPLITHASH_LSHASH_H
#define EXPERIMENT1_LAZYSPLITHASH_LSHASH_H

#include <iostream>
#include <vector>
#include "../Bucket.h"
#include "../../Auxizilary/GlobalVariable.h"
#include "../../Auxizilary/SysOutput.h"
#include "LSNode.h"


#define DEFAULT_BUCKETMAXSIZE 100
#define DEFAULT_TABLEBASESIZE 100
#define DEFAULT_IFTHRESHOLD   0.9

class LSbucket 
{
protected:
	std::vector<LSEntry> BucketEntries;
	uint32_t size,maxsize;
	PageType PageNum;


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
	std::vector<LSEntry> Getdata();
    int PageWrite();
	std::vector<LSEntry> BDataRead();

};


class LSHash
{

private:
	std::vector<LSbucket> bucketList;
	uint64_t Allsize;
	uint16_t bmaxsize;
	uint32_t SplitCursor;
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