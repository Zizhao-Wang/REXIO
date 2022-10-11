/* header files references. */
#include "ExHash.h"
#include <ctime>

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

    if(isFull())
    {
        vector<uint64_t> result;
        /*  for(it = values.begin(); it != values.end(); ++it)
            result.push_back(it->second);
        if(this->pageno == 100000000 )
            this->pageno = SSD_write2(result, true);
        else
            this->pageno = SSD_write2(result, false, pageno); */ 
        return 0;
    }
    //printf("%ld \n",pageno);
    this->pageno = SingleValueWrite(value,this->pageno,this->CurSize);
    values[key] = value;
    this->CurSize++;
    return 1;
}

inline int Bucket::remove(int key)
{
    std::map<int,uint64_t>::iterator it;
    it = values.find(key);
    if(it!=values.end())
    {
        values.erase(it);
        return 1;
    }
    else
    {
        cout<<"Cannot remove : This key does not exists"<<endl;
        return 0;
    }
}

inline int Bucket::update(int key, uint64_t value)
{
    std::map<int,uint64_t>::iterator it;
    it = values.find(key);
    if(it!=values.end())
    {
        values[key] = value;
        cout<<"Value updated"<<endl;
        return 1;
    }
    else
    {
        cout<<"Cannot update : This key does not exists"<<endl;
        return 0;
    }
}

ExEntry Bucket::Search(SKey key)
{

     // for(int i=0;i<bucket.size();i++)
     // {
     //      printf("%lu in bucket",bucket[i].key);
     // }
     // printf("============\n");
     // printf("PageNum: %lu ",PageNum);
     if(PageNum != UINT64_MAX )
     {
          std::vector<LHEntry> TempEntry = PageRead(PageNum);
          std::vector<LHEntry>::iterator get;
          get = find(TempEntry.begin(),TempEntry.end(), LHEntry{key,0});
          if(get != TempEntry.end())
          {
                    return  *get;  
          }
     }
     return LHEntry{0,0};
    
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

    assert(PageNum != UINT64_MAX)
   
    std::vector<ExEntry> TempEntry = PageRead();
    std::vector<ExEntry>::iterator get;
    get = find(TempEntry.begin(),TempEntry.end(), ExEntry{key,0});
    if(get != TempEntry.end())
    {
        return  *get;  
    }
    
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

inline int Bucket::getDepth(void)
{
    return depth;
}

inline int Bucket::increaseDepth(void)
{
    depth++;
    return depth;
}

inline int Bucket::decreaseDepth(void)
{
    depth--;
    return depth;
}

inline map<int, uint64_t> Bucket::copy(void)
{
    std::map<int, uint64_t> temp(values.begin(),values.end());
    return temp;
}

inline void Bucket::clear(void)
{
    values.clear();
}

inline void Bucket::display()
{
    std::map<int,uint64_t>::iterator it;
    for(it=values.begin();it!=values.end();it++)
        cout<<it->first<<" ";
    cout<<endl;
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