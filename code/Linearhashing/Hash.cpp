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
     hashtable;        // initialize a in-memory hash table
  
    /* Write datum */
    startTime = clock();
    for(int i=1;i<=3;i++)
    {
      uint64_t value = i;
        hash.insert(i,value,0);
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
