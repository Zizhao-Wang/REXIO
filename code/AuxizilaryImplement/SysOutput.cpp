#include <iostream>
#include "../Auxizilary/SysOutput.h"


std::string Uint64toString(uint64_t value)
{
    std::string result;
    result.resize(20);
    do
    {
        result += "0123456789"[value % 10];
        value /= 10;
    } while (value);
    std::reverse(result.begin(), result.end());
    
    return result;
}



void EMessageOutput(std::string error_msg, int ExitCode) 
{
    std::cerr << error_msg <<"\n";
    std::cerr << "Exiting..." <<"\n";
    exit(ExitCode);
}
