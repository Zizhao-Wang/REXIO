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


void EXHashing1();


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
    int increaseDepth(void);
    int decreaseDepth(void);

    void Allclear(void);
    void display(void);
    int  PageWrite();
    std::vector<ExEntry> Pageread();

};



class Directory {
    public:
        int global_depth, bucket_size;
        vector<Bucket*> buckets;
        int hash(int n);
        int pairIndex(int bucket_no, int depth);
        void grow(void);
        void shrink(void);
        void split(int bucket_no);
        void merge(int bucket_no);
        string bucket_id(int n);
    
        Directory(int depth, int bucket_size);
        void insert(int key,uint64_t value,bool reinserted);
        void remove(int key,int mode);
        void update(int key, uint64_t value);
        void search(int key);
        void display(bool duplicates);
};

/* Bucket class functions */







/* Directory class functions */

inline Directory::Directory(int depth, int bucket_size)
{
    this->global_depth = depth;
    this->bucket_size = bucket_size;
    for(int i = 0 ; i < 1<<depth ; i++ )
    {
        buckets.push_back(new Bucket(depth,bucket_size));
    }
}

// 将key变成块号
inline int Directory::hash(int n)
{
    return n&((1<<global_depth)-1);
}

// 返回该 块号 的局部深度位取反
inline int Directory::pairIndex(int bucket_no, int depth)
{
    //将这个bucket number的
    return bucket_no^(1<<(depth-1));
}

inline void Directory::grow(void)
{
    for(int i = 0 ; i < 1<<global_depth ; i++ )
        buckets.push_back(buckets[i]);
    global_depth++;
}

// 问题的突破点
//缺点在什么地方
// 逻辑 很小的问题
// 邮电在哪 


inline void Directory::shrink(void)
{
    int i,flag=1;
    for( i=0 ; i<buckets.size() ; i++ )
    {
        if(buckets[i]->getDepth()==global_depth)
        {
            flag=0;
            return;
        }
    }
    global_depth--;
    for(i = 0 ; i < 1<<global_depth ; i++ )
        buckets.pop_back();
}

inline void Directory::split(int bucket_no)
{
    int local_depth,pair_index,index_diff,dir_size,i;

    map<int, uint64_t> temp;
    map<int, uint64_t>::iterator it;

    // 哈希表倍增
    local_depth = buckets[bucket_no]->increaseDepth();
    if(local_depth>global_depth)
        grow();

    //传递该bucket number， 其后传递localdepth
    pair_index = pairIndex(bucket_no,local_depth);

    //对插入的数据进行初始化 
    buckets[pair_index] = new Bucket(local_depth,bucket_size);

    temp = buckets[bucket_no]->copy();
    buckets[bucket_no]->clear();


    index_diff = 1<<local_depth;
    dir_size = 1<<global_depth;


    for( i=pair_index-index_diff ; i>=0 ; i-=index_diff )
    {
        buckets[i] = buckets[pair_index];
        printf(" %d %d  %d\n",index_diff, dir_size,i);

    }
        

    for( i=pair_index+index_diff ; i<dir_size ; i+=index_diff )
    {
        buckets[i] = buckets[pair_index];

        printf(" %d %d  %d\n",index_diff, dir_size,i);
    }
        

    for(it=temp.begin();it!=temp.end();it++)
        insert((*it).first,(*it).second,1);
}

inline void Directory::merge(int bucket_no)
{
    int local_depth,pair_index,index_diff,dir_size,i;

    local_depth = buckets[bucket_no]->getDepth();
    pair_index = pairIndex(bucket_no,local_depth);
    index_diff = 1<<local_depth;
    dir_size = 1<<global_depth;

    if( buckets[pair_index]->getDepth() == local_depth )
    {
        buckets[pair_index]->decreaseDepth();
        delete(buckets[bucket_no]);
        buckets[bucket_no] = buckets[pair_index];
        for( i=bucket_no-index_diff ; i>=0 ; i-=index_diff )
            buckets[i] = buckets[pair_index];
        for( i=bucket_no+index_diff ; i<dir_size ; i+=index_diff )
            buckets[i] = buckets[pair_index];
    }
}

inline  string Directory::bucket_id(int n)
{
    int d;
    string s;
    d = buckets[n]->getDepth();
    s = "";
    while(n>0 && d>0)
    {
        s = (n%2==0?"0":"1")+s;
        n/=2;
        d--;
    }
    while(d>0)
    {
        s = "0"+s;
        d--;
    }
    return s;
}

inline void Directory::insert(int key,uint64_t value,bool reinserted)
{
    int bucket_no = hash(key);
    int status = buckets[bucket_no]->insert(key,value);

    // "status = 1" represent the successful insert operation.
    if(status==1)
    {
        //if(!reinserted)
            //cout<<"Inserted key "<<key<<" in bucket "<<bucket_id(bucket_no)<<endl;
        //else
            //cout<<"Moved key "<<key<<" to bucket "<<bucket_id(bucket_no)<<endl;
    }
    else if(status==0) // bucket 满了
    {
        split(bucket_no);
        insert(key,value,reinserted);
    }
    else
    {
        cout<<"Key "<<key<<" already exists in bucket "<<bucket_id(bucket_no)<<endl;
    }
}

inline void Directory::remove(int key,int mode)
{
    int bucket_no = hash(key);
    if(buckets[bucket_no]->remove(key))
        cout<<"Deleted key "<<key<<" from bucket "<<bucket_id(bucket_no)<<endl;
    if(mode>0)
    {
        if(buckets[bucket_no]->isEmpty() && buckets[bucket_no]->getDepth()>1)
            merge(bucket_no);
    }
    if(mode>1)
    {
        shrink();
    }
}

inline void Directory::update(int key, uint64_t value)
{
    int bucket_no = hash(key);
    buckets[bucket_no]->update(key,value);
}

inline void Directory::search(int key)
{
    int bucket_no = hash(key);
    cout<<"Searching key "<<key<<" in bucket "<<bucket_id(bucket_no)<<endl;
    buckets[bucket_no]->search(key);
}

inline void Directory::display(bool duplicates)
{
    int i,j,d;
    string s;
    std::set<string> shown;
    cout<<"Global depth : "<<global_depth<<endl;
    for(i=0;i<buckets.size();i++)
    {
        d = buckets[i]->getDepth();
        s = bucket_id(i);
        if(duplicates || shown.find(s)==shown.end())
        {
            shown.insert(s);
            for(j=d;j<=global_depth;j++)
                cout<<" ";
            cout<<s<<" => ";
            buckets[i]->display();
        }
    }
}




/* Main port */

#endif// port of extendible hashing 
