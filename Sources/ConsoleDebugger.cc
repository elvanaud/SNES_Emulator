#include "ConsoleDebugger.h"

ConsoleDebugger::ConsoleDebugger(W65816 & cpu) : cpu_debugger(cpu)
{

}

bool ConsoleDebugger::isExecutionBlocked()
{
    return false;
}

void ConsoleDebugger::continueExec()
{

}

bool ConsoleDebugger::tick()
{
    return false;
}
