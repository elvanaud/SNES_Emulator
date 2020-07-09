#include "AddressingMode.h"

AddressingMode::AddressingMode()
{
    //ctor
}

void AddressingMode::setStages(vector<vector<Stage>> st)
{
    stages = st;
}

vector<vector<Stage>> AddressingMode::gen(std::function<void(W65816*)> inst)
{
    for(unsigned int i = 0; i < stages.size(); ++i)
    {
        for(unsigned int j = 0; j < stages[i].size(); ++j)
        {
            if (stages[i][j].getSignal() == Stage::SIG_INST)
            {
                stages[i][j] = Stage(Stage::SIG_ALWAYS, inst);
            }
        }
    }
    return stages;
}

vector<std::function<void()>> AddressingMode::Signals()
{
    return signals;
}

void AddressingMode::setSignals(vector<std::function<void()>> s)
{
    signals = s;
}