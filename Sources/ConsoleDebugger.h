#ifndef CONSOLE_DEBUGGER_H
#define CONSOLE_DEBUGGER_H

#include "CPU/W65816.h"
#include "CPU/CPU_Debugger.h"

class ConsoleDebugger
{
public:
    ConsoleDebugger(W65816 & cpu);

    bool isExecutionBlocked();
    void continueExec();
    bool tick();

private:
    CPU_Debugger cpu_debugger;
};

#endif // CONSOLE_DEBUGGER_H
