#include "AddressingMode.h"

AddressingMode::AddressingMode(){}

AddressingMode::AddressingMode(int id)
{
    name = id;
    asmDecoder = std::function<void(std::stringstream&,string)>([](std::stringstream&st,string s){});//dummy decoder
}

void AddressingMode::setStages(vector<vector<Stage>> st)
{
    stages = st;

    for(unsigned int i = 0; i < stages.size(); ++i)
    {
        for(unsigned int j = 0; j < stages[i].size(); ++j)
        {
            if (stages[i][j].getEnablingCondition() == Stage::SIG_INST)
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

vector<std::function<void()>> AddressingMode::PredecodeSignals()
{
    return predecodeSignals;
}

void AddressingMode::setSignals(vector<std::function<void()>> s)
{
    signals = s;
}

void AddressingMode::setPredecodeSignals(vector<std::function<void()>> s)
{
    predecodeSignals = s;
}

int AddressingMode::Name()
{
    return name;
}

void AddressingMode::setASMDecoder(std::function<void(std::stringstream&,string)> decoder)
{
    asmDecoder = decoder;
}

string AddressingMode::decodeASM()
{
    std::stringstream st;
    //st<<" ";
    asmDecoder(st,"$");
    return st.str();
}
