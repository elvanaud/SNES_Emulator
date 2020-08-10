#include "CPU_Debugger.h"
#include <sstream>

CPU_Debugger::CPU_Debugger(W65816 & p_cpu) : cpu(p_cpu)
{

}

vector<string> split(const string& s, char delimiter)
{
   vector<string> tokens;
   string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

string csvGet(string src,string field)
{
    auto parts = split(src,'\n');
    int pos =0; int p = 0;
    for(auto f : split(parts[0],';'))
    {
        if(f==field)
            pos = p;
        p++;
    }
    return split(parts[1],';')[pos];
}

bool CPU_Debugger::cpuBlocked()
{
    if(!debuggerEnabled) return true;

    return false;
}

void CPU_Debugger::continueExec()
{

}

void CPU_Debugger::enableDebugger(bool enable)
{
    debuggerEnabled = enable;
}
