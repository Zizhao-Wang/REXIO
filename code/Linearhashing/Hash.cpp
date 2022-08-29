/* @Author: Zizhao Wang
 * @E-mail: zz.wang@siat.ac.cn
 * @Date: 10/8/2022
 * @name: Implementation of Linear-Hashing.
*/

//Some useful header files. 
#include<iostream>
#include<vector>
using namespace std;

typedef pair<int,int> II;
typedef vector<II> VII;
typedef vector<int> VI;
typedef vector< VI > VVI;
typedef long long int LL;

#define BucketSize 100  // size of a bucket
#define Tablebase  100  // In-memory table size

#define PB push_back
#define MP make_pair
#define F first
#define S second
#define SZ(a) (int)(a.size())
#define ALL(a) a.begin(),a.end()
#define SET(a,b) memset(a,b,sizeof(a))

#define si(n) scanf("%d",&n)
#define dout(n) printf("%d\n",n)
#define sll(n) scanf("%lld",&n)
#define lldout(n) printf("%lld\n",n)
#define fast_io ios_base::sync_with_stdio(false);cin.tie(NULL)
#define TRACE

#ifdef TRACE
#define trace(...) __f(#__VA_ARGS__, __VA_ARGS__)
template <typename Arg1>
void __f(const char* name, Arg1&& arg1){
  cerr<<name<<" : "<<arg1<<endl;
}
template <typename Arg1, typename... Args>
void __f(const char* names,Arg1&& arg1,Args&&... args){
  const char* comma=strchr(names+1,',');
  cerr.write(names,comma-names)<<" : "<<arg1<<" | ";__f(comma+1,args...);
}
#else
#define trace(...)
#endif

class Bucket
{
  public:
    vector<uint64_t>  bucket;
    uint64_t  BucketNo;  
}

class LinearHashTable
{
  /*
   * Private variables. 
   */
  private:
    vector<vector<Bucket>> BucketTable;  //In-memeory buckets table with fixed bucket size.

    uint64_t Tablesize;
    uint64_t mod;
    // vector<set<int>> Overflow;

  public:
  /*
   * some public function to manipulate private variables. 
   */
    LinearHashTable(int sz)
    {
      /*
       * Constructive function is used to initialize private variables. 
       */
      Tablesize = Tablebase;
      for(int i = 0; i<TableSize;i++)
      {
        Bucket TempBucket;
        Buckets.push_bakc(TempBucket);
      }
    }

    void split(int val)
    {
      Bucket.PB(set<int>());
      Overflow.PB(set<int>());
      for(auto v:Bucket[val])
      {
        if(v%(2*mod)==val+mod)
        {
          Bucket[val+mod].insert(v);
        }  
      }
        
      for(auto v:Bucket[val+mod])
      {
        Bucket[val].erase(v);
      }
        
      for(auto v:Overflow[val])
      if(v%(2*mod)==val+mod)  Overflow[val+mod].insert(v);
      for(auto v:Overflow[val+mod]) Overflow[val].erase(v);
    }

    void changemod()
    {
      for(int i=0;i<mod;i++) Bucket.PB(set<int>()),Overflow.PB(set<int>());
      mod*=2;marker=0;
    }

    int insert(int value)
    {

      /*
       * This function is implemented to insert a special value into a bucket according to
       * a special rule that usually is called “Linear Hashing method”. 
       * The following source code contains three steps:
       *  1. Find a proper bucket
       *  2. Insert this value into this in-memory bucket 
       *  4. Synchronize the value with Disk
       *  3. update in-memory table 
       */

      /*
       * step 1. Find a proper bucket for a specific value 
       */
      int bucketno = value % mod;  
      if(Buckets[bucketno].size() >= BucketSize)
      {
        bucketno = value % (mod*2);
      }
      if(Buckets[bucketno].count(x)+ Overflow[bucketno].count(x))
      {
        return 0;
      } 
      if(Bucket[bucketno].size()==bufsize) 
      {
        Overflow[bucketno].insert(x);
        split(marker++);
        if(marker==mod) changemod();
      }
      else Bucket[bucketno].insert(x);
      return 1;
    }
};

const int VV = int(1e9)+5;
vector<int> Input;
vector<int> Output;
void emptyOutput()
{
      reverse(ALL(Output));
      while(Output.size())
      {
        auto x = Output.back(); 
        Output.pop_back();
        dout(x-VV);
      }
      fflush(stdout);
}


void LHashPort()
{
  
    clock_t startTime,endTime;  // Definition of timestamp
    HashTable hashtable;        // initialize a in-memory hash table
  
    /* Write datum */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
      uint64_t value = i;
      // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of inserting: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


    /* Read datum */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
      uint64_t value = i;
      // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of reading data: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";

    /* data update */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
        uint64_t value = i;
        // d.insert(i,value,0);
    }
    endTime = clock();
    std::cout << "Total Time of datum update: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";
    
}
