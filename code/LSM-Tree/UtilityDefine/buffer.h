#include <set>
#include <vector>
#include "../../Auxizilary/VariablesTypes.h"


/*
 * Level 0 
 */
class Buffer 
{

public:
    int MaxSize;
    std::set<entry_t> Entries;
    Buffer(int maxsize) : MaxSize(maxsize) {};
    VAL_t * GetValue(KEY_t) const;
    std::vector<entry_t> * GetRange(KEY_t, KEY_t) const;
    bool PutValue(KEY_t, VAL_t val);
    void empty(void);

};
