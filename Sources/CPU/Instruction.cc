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

vector<std::function<void()>> Instruction::PredecodeSignals()
{
    return adrMode.PredecodeSignals();
}

AddressingMode Instruction::AdrMode()
{
    return adrMode;
}

bool Instruction::isIndexRelated()
{
    return indexRelated;
}

void Instruction::setIsIndexRelated(bool idxR)
{
    indexRelated = idxR;
}

string Instruction::getASM() const
{
    return opcodeASM; //TODO: add adrmode specific asm
}
