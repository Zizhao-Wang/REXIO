#include <iostream>
#include "../Auxizilary/SysOutput.h"

using namespace std;

void die(string error_msg) 
{
    cerr << error_msg << endl;
    cerr << "Exiting..." << endl;
    exit(EXIT_FAILURE);
}
