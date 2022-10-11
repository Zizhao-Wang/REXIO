/**
 * @date:   11/10/2022
 * @author: zz.wang
 * @details:
 *          This file contains all fuction declaration and definition of class used in "Lhash.cpp". 
 *
 * IDENTIFICATION:
 *          src/MultiHash/ExtendibleHash/ExHash.h
 **/

#ifndef EXPERIMENT1_EXTENDIBLEHASH_EXBUCKET_H
#define EXPERIMENT1_EXTENDIBLEHASH_EXBUCKET_H

#include <vector>

class Bucket 
{
private:
    int depth,size,maxsize; 
    uint64_t PageNum; //page number and current size 
    std::vector<ExEntry> values;

public:
    Bucket(int depth, int msize);

    int Insert(SKey key,SValue value);
    int Remove(SKey key);
    int Update(SKey key, SValue value);
    ExEntry Search(SKey key);

    bool IsFull(void);
    bool IsEmpty(void);

    int getDepth(void);
	std::vector<ExEntry> GetData();
    int increaseDepth(void);
    int decreaseDepth(void);

    void Allclear(void);
    void display(void);
    int  PageWrite();
    std::vector<ExEntry> Pageread();

};



#endif