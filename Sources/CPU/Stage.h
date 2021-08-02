#ifndef STAGE_H
#define STAGE_H

#include <functional>

#include "Types.h"

class W65816;
using namespace std::placeholders;

class Stage
{
public:
    enum EnablingCondition{
        SIG_ALWAYS,SIG_INST,SIG_DUMMY_STAGE,SIG_MEM16_ONLY,SIG_MODE16_ONLY,SIG_MODE8_ONLY,
        SIG_X_CROSS_PAGE,SIG_Y_CROSS_PAGE,SIG_DL_NOT_ZERO, SIG_INDIRECT_Y_CROSS_PAGE_OR_X16,
        SIG_PC_CROSS_PAGE_IN_EMUL, SIG_NATIVE_MODE, SIG_ACC_ZERO, SIG_ACC_NOT_ZERO, last};

    StageType get() const;
    EnablingCondition getEnablingCondition() const;

    template<class F, class ... Args>
    Stage(EnablingCondition s, F f, Args ... args)
    {
        enablingCondition = s;
        func = bind(f,_1,args...);
    }

private:
    StageType func;
    EnablingCondition enablingCondition;
};

#endif // STAGE_H
