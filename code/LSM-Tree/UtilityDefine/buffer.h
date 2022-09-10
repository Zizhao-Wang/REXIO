#include <set>
#include <vector>
#include "../../Auxizilary/GlobalVariable.h"


/*
 * Level 0 
 */
class Buffer 
{

public:
    int MaxSize;
    set<entry_t> Entries;
    Buffer(int maxsize) : MaxSize(maxsize) {};
    VAL_t * get(KEY_t) const;
    vector<entry_t> * range(KEY_t, KEY_t) const;
    bool put(KEY_t, VAL_t val);
    void empty(void);

};
