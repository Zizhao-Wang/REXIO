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
    uint64_t MaxSize;
    std::set<entry_t> Entries;

public:
    Buffer(size_t maxpage);
    bool PutValue(KEY_t, VAL_t val);
    VAL_t * GetValue(KEY_t) const;
    std::vector<entry_t> * GetRange(KEY_t, KEY_t) const;
    void AllClear(void);
    uint64_t GetMaxSize();
    std::set<entry_t> GetEntries();
};
