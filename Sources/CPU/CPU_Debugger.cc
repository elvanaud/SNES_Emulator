#include "CPU_Debugger.h"

CPU_Debugger::CPU_Debugger(W65816 & p_cpu) : cpu(p_cpu)
{

}

bool CPU_Debugger::cpuBlocked()
{
    if(!debuggerEnabled) return true;
}

void CPU_Debugger::continueExec()
{

}

void CPU_Debugger::enableDebugger(bool enable)
{
    debuggerEnabled = enable;
}
