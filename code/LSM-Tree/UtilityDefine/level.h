#include <queue>
#include "run.h"

class Level 
{
public:
    int LevelNumber;
    int MaxRuns;
    long MaxRunSize;
    std::deque<Run> Runs;
    Level(int n, long s) : MaxRuns(n), MaxRunSize(s) {}
    bool remaining(void) const 
    { 
        return max_runs - runs.size();
    }
};
