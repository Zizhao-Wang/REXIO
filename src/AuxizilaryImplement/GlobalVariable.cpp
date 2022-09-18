#include "../Auxizilary/GlobalVariable.h"

long LevelNum = 1;

long LevelAlloctor()
{
	return LevelNum++;
}