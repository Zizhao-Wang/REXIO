#ifndef EXPERIMENT1_HEADER_H
#define EXPERIMENT1_HEADER_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

/*
 * Hash function. 
 */
class Hash
{
public:
    virtual void add(std::string& value) = 0;
    virtual bool check(std::string& value) = 0;
    virtual int getMaxCollisions() const = 0;
};



// class HashTable:public Hash 
// {

//     int max_col;
//     // vector<string> _vec;

// public:

//    /* 
//     * Here are some useful functions for updating hash table:
//     * 1. Inserting a new key/value data pair.
//     * 2. Deleting a Key/Value item.
//     * 3. Searching one or more specific value entries. 
//     */

//     int insertion(uint64_t value)
//     {
        
//     }

//     int deletion(uint64_t key)
//     {
        
//     }

//     int Searching(uint64_t key)
//     {
        
//     }

//     // HashTable() : _vec(100), max_col(0) {}

//     // void add(std::string &val) 
//     // {
//     //     int tmp = 0;
//     //     int res = val[0] - 97;
//     //     if (_vec[res].empty()) {
//     //         _vec[res] = val;
//     //     } else {
//     //         while (!_vec[res].empty()) {
//     //             ++tmp;
//     //             ++res;
//     //         }
//     //         _vec[res] = val;
//     //     }
//     //     if (tmp > max_col)
//     //         max_col = tmp;
//     // }

//     // bool check(std::string &val) 
//     // {
//     //     int res = val[0] - 97;
//     //     if (find(_vec.begin(), _vec.end(), val) != _vec.end())
//     //         return true;
//     //     else {
//     //         while (!_vec[res].empty()) {
//     //             ++res;
//     //             if (find(_vec.begin(), _vec.end(), val) != _vec.end())
//     //                 return true;
//     //         }
//     //         return false;
//     //     }
//     // }

//     // int getMaxCollisions() const 
//     // {
//     //     return max_col;
//     // }
// };

// /* Declaration of L-Hashing functions */
// void LinearHashing(); // port of L-Hashing



#endif //CLASSWORK_HEADER_H