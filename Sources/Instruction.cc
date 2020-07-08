#include "Instruction.h"

/*Instruction::Instruction(string opcodeASM, AddressingMode &adrMode, void(W65816::*inst)())
{
    opcodeASM = opcodeASM;
    stages = adrMode.gen(inst);
    adrMode = adrMode;
}*/

Instruction::Instruction(){}

vector<vector<Stage>> Instruction::Stages()
{
    return stages;
}

vector<std::function<void()>> Instruction::Signals()
{
    return adrMode.Signals();
}
