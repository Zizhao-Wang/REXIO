#ifndef EXPERIMENT1_HEADER_H
#define EXPERIMENT1_HEADER_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

class Hash
{
public:
    virtual void add(std::string& value) = 0;
    virtual bool check(std::string& value) = 0;
    virtual int getMaxCollisions() const = 0;
};



/* Declaration of L-Hashing functions */
void LinearHashing(); // port of L-Hashing



#endif //CLASSWORK_HEADER_H