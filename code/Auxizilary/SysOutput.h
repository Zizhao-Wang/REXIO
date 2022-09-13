#ifndef EXPERIMENT1_SYSOUTPUT_H
#define EXPERIMENT1_SYSOUTPUT_H

#include <cstring>
using namespace std;

void EMessageOutput(string error_msg, int ExitCode);


#define AssertCondition(expr)   \
    (static_cast <bool> (expr)	\
    ? void (0)				\
    : EMessageOutput("Assertion aborted!",105))

#endif