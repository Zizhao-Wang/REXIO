#include "LSM-Tree/UtilityDefine/ResAllocate.h"

long LevelNum = 1;

long LevelAlloctor()
{
	return LevelNum++;
}