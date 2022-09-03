/* @Author: Zizhao Wang
 * @E-mail: zz.wang@siat.ac.cn
 * @Date: 10/8/2022
 * @name: Implementation of Linear-Hashing.
*/

//Some useful header files.

#include<iostream>
#include<vector>
#include "./LHash.h"



/* Global variables definition and utilization, coming soon...  */

void LHashPort()
{
  
    clock_t startTime,endTime;  // Definition of timestamp
    LinearHashTable hashtable;        // initialize a in-memory hash table
  
    /* Write datum */
    startTime = clock();
    for(int i=0;i<=1000000;i++)
    {
      uint64_t value = i;
      hashtable.insert(i,value);
    }
    endTime = clock();
    std::cout << "Total Time of inserting: " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s\n";


    /* Read datum */
    startTime = clock();
    for(int i=1;i<=1000000;i++)
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
