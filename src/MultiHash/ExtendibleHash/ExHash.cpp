/* header files references. */
#include "ExHash.h"
#include <ctime>
#include "../../Backend/SSDRead/reader.h"


Bucket::Bucket(int depth, int msize)
{
    this->depth = depth;
    this->maxsize = msize;
    this->size = 0;
    this->PageNum = UINT64_MAX;
}

int Bucket::Insert(SKey key,uint64_t value)
{


    std::vector<ExEntry>::iterator it;
    it = find(values.begin(),values.end(),ExEntry{key,0}); 

    if(it!=values.end())
    {
        (*it).val = value;
        return 1;
    }

    values.emplace_back(ExEntry{key,value});

    if(IsFull())
    {
        PageNum = PageWrite();
        return 0;
    }
    //printf("%ld \n",pageno);
    return 1;
}

int Bucket::Remove(SKey key)
{
    if(values.size()!=0)
     {
          std::vector<ExEntry>::iterator get;
          get = find(values.begin(),values.end(), LHEntry{key,0});
          if(get != values.end())
          {
               values.erase(get);
               size--;
               return 0;
          }
     }
     else
     {
        values = EBucketRead(PageNum);
        std::vector<ExEntry>::iterator get;
        size += maxsize;
        get = find(values.begin(),values.end(), LHEntry{key,0});
        if(get != values.end())
        {
            values.erase(get);
            size--;
            return 0;
        }
     }

     return 1;
}

inline int Bucket::Update(SKey key, SValue value)
{
    if(values.size()!=0)
     {
          std::vector<ExEntry>::iterator get;
          get = find(values.begin(),values.end(), LHEntry{key,0});
          if(get != values.end())
          {
               (*get).val = value;
               return 0;
          }
    }
    else
    {
        values = EBucketRead(PageNum);
        std::vector<ExEntry>::iterator get;
        size += maxsize;
        get = find(values.begin(),values.end(), LHEntry{key,0});
        if(get != values.end())
        {
            (*get).val = value;
            PageNum = PageWrite();
            return 0;
        }
    }
     return 1;
}

ExEntry Bucket::Search(SKey key)
{
     // for(int i=0;i<bucket.size();i++)
     // {
     //      printf("%lu in bucket",bucket[i].key);
     // }
     // printf("============\n");
     // printf("PageNum: %lu ",PageNum);
    
    if(values.size()!=0)
    {
        std::vector<ExEntry>::iterator it;
        it = find(values.begin(),values.end(), ExEntry{key,0});
        if(it != values.end())
        {
            return *it;
        }
        return ExEntry{0,0};
    }

    assert(PageNum != UINT64_MAX);
   
    std::vector<ExEntry> TempEntry = EBucketRead(PageNum);
    std::vector<ExEntry>::iterator it;
    it = find(TempEntry.begin(),TempEntry.end(), ExEntry{key,0});

    if(it != TempEntry.end())
    {
        return  *it;  
    }
    
    return ExEntry{key,0};
}

bool Bucket::IsFull(void)
{
    if(size >= maxsize)
        return true;
    else
        return false;
}

bool Bucket::IsEmpty(void)
{
    if(size == 0)
        return true;
    else
        return false;
}

int Bucket::getDepth(void)
{
    return depth;
}

int Bucket::increaseDepth(void)
{
    depth++;
    return depth;
}

int Bucket::decreaseDepth(void)
{
    depth--;
    return depth;
}

void Bucket::Allclear(void)
{
    values.clear();
    size = 0;
}

std::vector<ExEntry> Bucket::Pageread()
{
    std::vector<ExEntry> data;
    return data;
}




void EXHashing1()
{
    bool show_messages, show_duplicate_buckets;
    int bucket_size=2048, initial_global_depth=1;
    int key, mode;
    string choice, value;
    clock_t startTime,endTime;

    // Set show_messages to 0 when taking input using file redirection
    show_messages = 0;

    // Set show_duplicate_buckets to 1 to see all pointers instead of unique ones
    show_duplicate_buckets = 0;

    Directory d(initial_global_depth,bucket_size);
    printf("======Initialized directory structure completed!=======\n");

    /* Insert 10 values 
     */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
        uint64_t value = i;
        d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time : " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;

 

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