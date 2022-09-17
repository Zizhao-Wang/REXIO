#include "../Auxizilary/GlobalVariable.h"

int LevelNum = 1;

int LevelAlloctor()
{
	printf("Level number %d\n",LevelNum);
	return LevelNum++;
}