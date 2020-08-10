#ifndef ADDRESSINGMODE_H
#define ADDRESSINGMODE_H

#include <sstream>
#include <string>
using std::string;
#include <vector>
using std::vector;

#include "Types.h"
#include "Stage.h"


class AddressingMode
{
public:
    AddressingMode();
    AddressingMode(int id);

    vector<vector<Stage>> gen(std::function<void(W65816*)>);
    void setStages(vector<vector<Stage>> stages);
    vector<std::function<void()>> Signals();
    vector<std::function<void()>> PredecodeSignals();
    void setSignals(vector<std::function<void()>> s);
    void setPredecodeSignals(vector<std::function<void()>> s);
    int Name();

    void setASMDecoder(std::function<void(std::stringstream&)> decoder);
    string decodeASM();

private:
    vector<vector<Stage>> stages;
    vector<std::function<void()>> signals;
    vector<std::function<void()>> predecodeSignals;

    unsigned int pipelineStage;
    unsigned int stageNum;
    int name = -1;
    std::function<void(std::stringstream&)> asmDecoder;
};

#endif // ADDRESSINGMODE_H
