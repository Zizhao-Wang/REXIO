#include <iostream>
#include "../Auxizilary/SysOutput.h"

using namespace std;

void EMessageOutput(string error_msg, int ExitCode) 
{
    cerr << error_msg << endl;
    cerr << "Exiting..." << endl;
    exit(ExitCode);
}
