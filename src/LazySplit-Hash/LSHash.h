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

class LSHash
{

private:
	/* data */
public:
	LSHash(/* args */);

	int Insert(uint64_t key, uint64_t value);
	uint64_t Search(uint64_t key);
	int Delete(uint64_t key);
	~LSHash();
	
};







#endif //EXPERIMENT1_LAZYSPLITHASH_H