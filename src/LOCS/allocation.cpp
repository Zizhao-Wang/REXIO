#include "allocation.h"

long LevelNumber = 1;

long LevelRising()
{
	return LevelNumber++;
}