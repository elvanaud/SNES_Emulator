#include "AddressingMode.h"

AddressingMode::AddressingMode(){}

AddressingMode::AddressingMode(int id)
{
    name = id;
}

void AddressingMode::setStages(vector<vector<Stage>> st)
{
    stages = st;

    for(unsigned int i = 0; i < stages.size(); ++i)
    {
        for(unsigned int j = 0; j < stages[i].size(); ++j)
        {
            if (stages[i][j].getSignal() == Stage::SIG_INST)
            {
                pipelineStage = i;
                stageNum = j;
            }
        }
    }
}

vector<vector<Stage>> AddressingMode::gen(std::function<void(W65816*)> inst)
{
    stages[pipelineStage][stageNum] = Stage(Stage::SIG_ALWAYS, inst);
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

int AddressingMode::Name()
{
    return name;
}
