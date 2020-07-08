#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
using std::vector;


#include "Stage.h"


class Instruction
{
public:
    Instruction();
    vector<vector<Stage>> Stages();

private:
    vector<vector<Stage>> stages;
};

#endif // INSTRUCTION_H
