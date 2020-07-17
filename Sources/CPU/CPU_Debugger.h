#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "W65816.h"

class CPU_Debugger
{
public:
    CPU_Debugger(W65816 & p_cpu);
    void continueExec();
    bool cpuBlocked();
    void enableDebugger(bool enable = true);
private:
    bool debuggerEnabled = false;
    W65816 & cpu;
};

#endif // DEBUGGER_H
