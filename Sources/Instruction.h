#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
using std::vector;
#include <string>
using std::string;

#include "Stage.h"
#include "AddressingMode.h"

class Instruction
{
public:
    Instruction();
    template<class F,class ... Args>
    Instruction(string opcodeASM, AddressingMode &adrMode,F inst,Args ... args)
    {
        opcodeASM = opcodeASM;
        stages = adrMode.gen(bind(inst,_1,args...));
        adrMode = adrMode;
    }

    vector<vector<Stage>> Stages();
    vector<std::function<void()>> Signals();

private:
    vector<vector<Stage>> stages;
    AddressingMode adrMode;
    string opcodeASM;
};

#endif // INSTRUCTION_H
