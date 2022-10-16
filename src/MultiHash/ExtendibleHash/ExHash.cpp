/* header files references. */
#include "ExHash.h"
#include <ctime>
#include "ExNode.h"
#include "../../Backend/SSDRead/reader.h"

int Eread = 0;
int Ewrite = 0;
int Eerase = 0;

Directory::Directory(int depth, int bucket_size)
{
    this->global_depth = depth;
    this->bucket_size = bucket_size;

    for(int i = 0 ; i < 1<<depth ; i++ )
    {
        buckets.push_back(new ExBucket(depth,bucket_size));
    }
}


int Directory::hash(int n)
{
    return n&((1<<global_depth)-1);
}


int Directory::pairIndex(int bucket_no, int depth)
{
    return bucket_no^(1<<(depth-1));
}

void Directory::grow(void)
{
    for(int i = 0 ; i < 1<<global_depth ; i++ )
    {
        buckets.push_back(buckets[i]);
    }
        
    global_depth++;
}

// 问题的突破点
//缺点在什么地方
// 逻辑 很小的问题
// 邮电在哪 


void Directory::shrink(void)
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

    std::vector<ExEntry> temp;
    std::vector<ExEntry>::iterator it;

    // Hash Table double
    local_depth = buckets[bucket_no]->increaseDepth();
    if(local_depth>global_depth)
        grow();

    //传递该bucket number， 其后传递localdepth
    pair_index = pairIndex(bucket_no,local_depth);

    //对插入的数据进行初始化 
    buckets[pair_index] = new ExBucket(local_depth,bucket_size);

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

void Directory::merge(int bucket_no)
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

string Directory::bucket_id(int n)
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
    //printf("bucket no:%d\n",bucket_no);
    if(buckets[bucket_no]->IsFull()) // chech if bucket is full or not.
    {
        split(bucket_no);
        insert(key,value);
    }
    else
    {
        buckets[bucket_no]->Insert(key,value);
    }
    
}

void Directory::remove(SKey key)
{
    int bucket_no = hash(key);
    buckets[bucket_no]->Remove(key);

    if(buckets[bucket_no]->IsEmpty() && buckets[bucket_no]->getDepth()>1)
        merge(bucket_no);
    
    //shrink();
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
        //printf("Not found!\n");
    }

}

void Directory::display()
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
    clock_t startTime,endTime;

    // Set show_messages to 0 when taking input using file redirection
    show_messages = 0;

    // Set show_duplicate_buckets to 1 to see all pointers instead of unique ones
    show_duplicate_buckets = 0;

    Directory d(initial_global_depth,bucket_size);
    printf("======Initialized directory structure completed!=======\n");


    /* workload a: insert only*/
    printf(("=========workload A=====\n"));
    startTime = clock();
    for(SKey i=1;i<=40000000;i++)
    {
        if(i%10000000==0||i==1000000)
        {
            endTime = clock();
            std::cout << "Total Time of workload A: "<<i <<"  " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase );
        }
        d.insert(i,i);;
    }
    endTime = clock();
    printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite,Eerase );
    std::cout << "Total Time of workload A: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
    //d.display();


    /* workload b: read only, all in it */
    startTime = clock();
    Eread = 0;
    Ewrite = 0;
    Eerase = 0;
    printf(("=========workload B=====\n"));
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        SKey k = 1+(rand()%40000000);
        d.search(k);
        if(i==10000 || i%100000==0)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase );   
        }
    }
    endTime = clock();
    printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase );
    std::cout << "Total Time of workload B: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

    /* workload c: read only, 50% in it, 50% not in it */
    startTime = clock();
    printf(("=========workload C=====\n"));
    Eread = 0;
    Ewrite = 0;
    Eerase = 0;
     
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        if(i%100<50)
        {
            SKey k = 1+(rand()%40000000);
            d.search(k);
        }
        else
        {
            SKey k = 40000000+(rand()%40000000);
            d.search(k);
        }
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";  
            printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase );   
        }
    }
    printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase );
    endTime = clock();
    std::cout << "Total Time of workload C: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

    /* workload d: update heavy workload, 50% read, 50% update */
    startTime = clock();
    printf(("=========workload D=====\n"));
    Eread = 0;
    Ewrite = 0;
    Eerase = 0;
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        if(i%2==0)
        {
            SKey k = 1+(rand()%40000000);
            d.search(k);
        }
        else
        {
            SKey k = 1+(rand()%40000000);
            d.update(k,k+1);
        }
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload D: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase ); 
        } 
    }
    printf("Read count:%d Write count:%d Erase Count:%d \n",Eread,Ewrite ,Eerase );
    endTime = clock();
    std::cout << "Total Time of workload d: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

    /* workload E: read mostly workload, 95% read, 5% update */
    Eread = 0;
    Ewrite = 0;
    Eerase = 0;
    startTime = clock();
    printf(("=========workload E=====\n"));
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        if(i%100<95)
        {
            SKey k = 1+(rand()%40000000);
            d.search(k);
        }
        else
        {
            SKey k = 1+(rand()%40000000);
            d.update(k,k+1);
        }
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n"; 
            printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase );  
        } 
    }
    printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase);
    endTime = clock();
    std::cout << "Total Time of workload E: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

     /* workload F: read latest workload, 95% read, 5% insert */
    startTime = clock();
    printf(("=========workload F=====\n"));
    Eread = 0;
    Ewrite = 0;
    Eerase = 0;
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        if(i%100<95)
        {
            SKey k = 1+(rand()%40000000);
            d.search(k);
        }
        else
        {
            SKey k = 1+(rand()%40000000);
            d.insert(i+40000000,i+40000000);
        }
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase);
        } 
    }
    printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase);
    endTime = clock();
    std::cout << "Total Time of workload F: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";

    /* workload G: delete workload, 100% delete*/
    startTime = clock();
    printf(("=========workload G=====\n"));
    Eread = 0;
    Ewrite = 0;
    Eerase = 0;
    for(int i=1;i<=1000000;i++)
    {
        srand48(time(NULL));
        SKey k = 1+(rand()%40000000);
        d.remove(k);
        if(i%100000==0 || i==10000)
        {
            endTime = clock();
            std::cout << "Total Time of "<<i<<" in workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
            printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase);     
        }  
    }
    printf("Read count:%d Write count:%d Erase Count:%d \n",Eread ,Ewrite ,Eerase);
    endTime = clock();
    std::cout << "Total Time of workload G: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n\n";
 

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