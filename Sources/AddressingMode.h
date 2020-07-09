#ifndef ADDRESSINGMODE_H
#define ADDRESSINGMODE_H

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
    void setSignals(vector<std::function<void()>> s);
    int Name();
private:
    vector<vector<Stage>> stages;
    vector<std::function<void()>> signals;

    unsigned int pipelineStage;
    unsigned int stageNum;
    int name = -1;
};

#endif // ADDRESSINGMODE_H
