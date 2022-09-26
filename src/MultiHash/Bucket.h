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

class bucket 
{

private:
	uint16_t CurrentSize, MaxSize; //page number and current size 
	uint64_t PageNo;
	

public:

    int depth; 
    map<int, uint64_t> values;
    bucket(int depth, int size);

   /**
	* As a rule, a complete index structure should at least include the methods for CURD and crash recovery.
	* Create, update, retrieval, delete and crash recovery.
	**/
    int insert(int key,uint64_t value);
    int remove(int key);
    int update(int key, uint64_t value);
    void search(int key);

    int isFull(void);
    int isEmpty(void);

    int getDepth(void);
    int increaseDepth(void);
    int decreaseDepth(void);
    std::map<int, uint64_t> copy(void);
    void clear(void);
    void display(void);
    int  PageWrite();

};


#endif //EXPERIMENT1_LAZYSPLITHASH_H