#include "AddressingMode.h"

AddressingMode::AddressingMode()
{
    //ctor
}

void AddressingMode::setStages(vector<vector<Stage>> &&stages)
{
    stages = stages;
}

vector<vector<Stage>> AddressingMode::gen(std::function<void(W65816*)>)
{
    return stages;
}

vector<std::function<void()>> AddressingMode::Signals()
{
    return signals;
}

void AddressingMode::setSignals(vector<std::function<void()>> &&s)
{
    signals = s;
}
