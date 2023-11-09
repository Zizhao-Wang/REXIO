#include <iostream>
#include "include/sys_output.h"


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



void EMessageOutput(const std::string &error_msg, int ExitCode) 
{
    std::cerr << "\x1b[31mError: " << error_msg << "\x1b[0m" << std::endl;  // Added red color for error messages
    std::cerr << "\x1b[31mExiting...\x1b[0m" << std::endl;  // Added red color for the "Exiting..." message
    exit(ExitCode);
}

