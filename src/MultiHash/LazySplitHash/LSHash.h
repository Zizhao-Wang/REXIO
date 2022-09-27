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
    

    bool IsFull(void) const ;
    bool IsEmpty(void) const ;

    void AllClear(void) ;
    int PageWrite() ;

};


class LSHash
{

private:
	std::vector<LSbucket> bucketList;
public:
	LSHash(/* args */);

	int Insert(uint64_t key, uint64_t value);
	uint64_t Search(uint64_t key);
	int Delete(uint64_t key);
	~LSHash();
	
};







#endif //EXPERIMENT1_LAZYSPLITHASH_H