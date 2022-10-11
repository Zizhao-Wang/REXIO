/**
 * @date:   5/8/2022
 * @author: zz.wang
 * @details:
 *          This file contains all fuction declaration and definition of class used in "Lhash.cpp". 
 *
 * IDENTIFICATION:
 *          src/MultiHash/ExtendibleHash/ExHash.h
 **/

#ifndef EXPERIMENT1_EXTENDIBLEHASH_EXHASH_H
#define EXPERIMENT1_EXTENDIBLEHASH_EXHASH_H

#include <iostream>
#include <vector>
#include "ExNode.h"
#include "../../Backend/IODisk/WriteDisk.h"
#include "ExBucket.h"

void EXHashing1();


class Directory 
{
private:
    int global_depth, bucket_size;
    vector<Bucket*> buckets;
    
public:
    int hash(int n);
    int pairIndex(int bucket_no, int depth);
    void grow(void);
    void shrink(void);
    void split(int bucket_no);
    void merge(int bucket_no);
    string bucket_id(int n);
    
    Directory(int depth, int bucket_size);
    void insert(SKey key,SValue value);
    void remove(SKey key,int mode);
    void update(SKey key, SValue value);
    void search(SKey key);
    void display(bool duplicates);


};

/* Bucket class functions */







/* Directory class functions */






/* Main port */

#endif// port of extendible hashing 
