#include <set>
#include <vector>
#include "../../Auxizilary/VariablesTypes.h"
#include "../../Backend/IODisk/WriteDisk.h"

/*
 * Level 0 
 */
class Buffer 
{
private:
    int MaxSize;
    std::set<entry_t> Entries;

public:
    Buffer(int maxpage);
    bool PutValue(KEY_t, VAL_t val);
    VAL_t * GetValue(KEY_t) const;
    std::vector<entry_t> * GetRange(KEY_t, KEY_t) const;
    void empty(void);
    int GetMaxSize();
    std::set<entry_t> GetEntries();
};
