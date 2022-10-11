/* header files references. */
#include "ExHash.h"
#include <ctime>
#include "../../Backend/SSDRead/reader.h"

inline Directory::Directory(int depth, int bucket_size)
{
    this->global_depth = depth;
    this->bucket_size = bucket_size;
    for(int i = 0 ; i < 1<<depth ; i++ )
    {
        buckets.push_back(new Bucket(depth,bucket_size));
    }
}


int Directory::hash(int n)
{
    return n&((1<<global_depth)-1);
}


inline int Directory::pairIndex(int bucket_no, int depth)
{
    return bucket_no^(1<<(depth-1));
}

void Directory::grow(void)
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

void Directory::split(int bucket_no)
{

    int local_depth,pair_index,index_diff,dir_size,i;

    std::vector<ExEntry> temp ;
    std::vector<ExEntry>::iterator it;

    // Hash Table double
    local_depth = buckets[bucket_no]->increaseDepth();
    if(local_depth>global_depth)
        grow();

    //传递该bucket number， 其后传递localdepth
    pair_index = pairIndex(bucket_no,local_depth);

    //对插入的数据进行初始化 
    buckets[pair_index] = new Bucket(local_depth,bucket_size);

    temp = buckets[bucket_no]->GetData();
    buckets[bucket_no]->Allclear();


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
        insert((*it).key,(*it).val);
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

void Directory::insert(SKey key,SValue value)
{
    int bucket_no = hash(key);
    
    if(buckets[bucket_no]->IsFull()) // chech if bucket is full or not.
    {
        split(bucket_no);
    }

    insert(key,value);
}

void Directory::remove(SKey key,int mode)
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

void Directory::update(SKey key, SValue value)
{
    int bucket_no = hash(key);
    buckets[bucket_no]->Update(key,value);
}

void Directory::search(SKey key)
{

    int bucket_no = hash(key);
    ExEntry entry;
    entry = buckets[bucket_no]->Search(key);
    if(entry.key!=key)
    {
        printf("Not found!\n");
    }

}

void Directory::display(bool duplicates)
{

    cout<<"Global depth : "<<global_depth<<endl;
    for(int i=0;i<buckets.size();i++)
    {
        int d = buckets[i]->getDepth();
        printf("The %d bucket has depth:%d.",i,d);
        buckets[i]->display();
        printf("\n");
    }
}



void EXHashing1()
{
    bool show_messages, show_duplicate_buckets;
    int bucket_size=CalculatePageCapacity(sizeof(ExEntry)); 
    int initial_global_depth=2;
    int key, mode;
    string choice, value;
    clock_t startTime,endTime;

    // Set show_messages to 0 when taking input using file redirection
    show_messages = 0;

    // Set show_duplicate_buckets to 1 to see all pointers instead of unique ones
    show_duplicate_buckets = 0;

    Directory d(initial_global_depth,bucket_size);
    printf("======Initialized directory structure completed!=======\n");

    /* WorkLoad A: */
    startTime = clock();
    for(int i=1;i<=1000000;i++)
    {
        d.insert(i,i);
    }
    endTime = clock();
    std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;

 

    /*do
    {
        cout<<endl;
        if(show_messages) { cout<<">>> "; }
        cin>>choice;
        if(choice=="insert")
        {
            cin>>key>>value;
            if(show_messages) { cout<<endl; }
            d.insert(key,value,0);
        }
        else if(choice=="delete")
        {
            cin>>key>>mode;
            if(show_messages) { cout<<endl; }
            d.remove(key,mode);
        }
        else if(choice=="update")
        {
            cin>>key>>value;
            if(show_messages) { cout<<endl; }
            d.update(key,value);
        }
        else if(choice=="search")
        {
            cin>>key;
            if(show_messages) { cout<<endl; }
            d.search(key);
        }
        else if(choice=="display")
        {
            if(show_messages) { cout<<endl; }
            d.display(show_duplicate_buckets);
        }
    } while(choice!="exit");*/
}