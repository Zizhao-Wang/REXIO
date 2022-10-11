#include "ExBucket.h"

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

std::vector<ExEntry>  Bucket::GetData()
{

    //assert(size == CalculatePageCapacity(sizeof(ExEntry)));
    if(PageNum != UINT64_MAX && values.size()==0)
    {
        std::vector<ExEntry> entries = EBucketRead(PageNum);
        return entries;
    }
    //printf("test! in Get BUCKET\n");
    return values;

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