#include "CPU/W65816.h"
#include "CPU/CPU_Debugger.h"

class ConsoleDebugger
{
public:
    ConsoleDebugger(W65816 & cpu);

    bool isExecutionBlocked();
    void continueExec();

private:
    CPU_Debugger cpu_debugger;
};
