#include "ExBucket.h"
#include <algorithm>
#include "../../Backend/SSDWrite/writer.h"
#include "../../Backend/SSDRead/reader.h"

ExBucket::ExBucket(int depth, int msize)
{
    this->depth = depth;
    this->maxsize = msize;
    this->size = 0;
    this->PageNum = UINT64_MAX;
}

int ExBucket::PageWrite()
{
    sort(values.begin(),values.end());
    PageNum = SingleBucketWrite(values, PageNum);
    values.clear();
    return 0;
}

int ExBucket::Insert(SKey key,uint64_t value)
{

    std::vector<ExEntry>::iterator it;
    it = find(values.begin(),values.end(),ExEntry{key,0}); 

    if(it!=values.end())
    {
        (*it).val = value;
        return 0;
    }

    values.emplace_back(ExEntry{key,value});
    size++;

    if(IsFull())
    {
        PageWrite();
        return 0;
    }
    //printf("%ld \n",pageno);
    return 1;
}

int ExBucket::Remove(SKey key)
{
    if(values.size()!=0)
     {
          std::vector<ExEntry>::iterator get;
          get = find(values.begin(),values.end(), ExEntry{key,0});
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
        get = find(values.begin(),values.end(), ExEntry{key,0});
        if(get != values.end())
        {
            values.erase(get);
            size--;
            return 0;
        }
     }

     return 1;
}

int ExBucket::Update(SKey key, SValue value)
{
    if(values.size()!=0)
     {
          std::vector<ExEntry>::iterator get;
          get = find(values.begin(),values.end(), ExEntry{key,0});
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
        get = find(values.begin(),values.end(), ExEntry{key,0});
        if(get != values.end())
        {
            (*get).val = value;
            PageWrite();
            return 0;
        }
    }
     return 1;
}

ExEntry ExBucket::Search(SKey key)
{
     // for(int i=0;i<ExBucket.size();i++)
     // {
     //      printf("%lu in ExBucket",ExBucket[i].key);
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

bool ExBucket::IsFull(void)
{
    if(size >= maxsize)
        return true;
    else
        return false;
}

bool ExBucket::IsEmpty(void)
{
    if(size == 0)
        return true;
    else
        return false;
}

int ExBucket::getDepth(void)
{
    return depth;
}

std::vector<ExEntry>  ExBucket::GetData()
{

    //assert(size == CalculatePageCapacity(sizeof(ExEntry)));
    if(PageNum != UINT64_MAX && values.size()==0)
    {
        std::vector<ExEntry> entries = EBucketRead(PageNum);
        return entries;
    }
    //printf("test! in Get ExBucket\n");
    return values;

}

int ExBucket::increaseDepth(void)
{
    depth++;
    return depth;
}

int ExBucket::decreaseDepth(void)
{
    depth--;
    return depth;
}

void ExBucket::Allclear(void)
{
    values.clear();
    size = 0;
}

void ExBucket::display(void)
{
    printf("Memeory size:%lu Actual size:%d PageNum:%lu",values.size(),size,PageNum);
    return ;
}

std::vector<ExEntry> ExBucket::Pageread()
{
    std::vector<ExEntry> data;
    return data;
}