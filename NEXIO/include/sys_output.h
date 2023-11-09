/**
 * @date   17/9/2022
 * @author zz.wang
 * @details
 *          This file shows how to construct a LSM-Tree.
 *
 * IDENTIFICATION:
 *          code/Auxizilary/SysOutput.h
 **/

#ifndef EXPERIMENT1_SYSOUTPUT_H
#define EXPERIMENT1_SYSOUTPUT_H

#include <string>
#include <algorithm>

std::string Uint64toString(uint64_t value);

/**
 * \brief Outputs an error message and exits the program.
 * 
 * This function displays a provided error message to the standard error stream,
 * notifies the user that the program is exiting, and then exits with a specified
 * exit code.
 * 
 * \param error_msg The error message to be displayed.
 * \param ExitCode The exit code with which the program should terminate.
 */
void EMessageOutput(const std::string &error_msg, int ExitCode);



#define AssertCondition(expr)   \
    (static_cast <bool> (expr)	\
    ? void (0)				\
    : EMessageOutput("Assertion aborted!",105))

#endif // #endif