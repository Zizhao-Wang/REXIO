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

#include <cstring>
#include <algorithm>

std::string Uint64toString(uint64_t value);


void EMessageOutput(std::string error_msg, int ExitCode);



#define AssertCondition(expr)   \
    (static_cast <bool> (expr)	\
    ? void (0)				\
    : EMessageOutput("Assertion aborted!",105))

#endif // #endif