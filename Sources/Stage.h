#ifndef STAGE_H
#define STAGE_H

#include <functional>

#include "Types.h"

class W65816;
using namespace std::placeholders;

class Stage
{
public:
    enum Signal{SIG_ALWAYS,SIG_INST,SIG_MEM16_ONLY};

    StageType get() const;
    Signal getSignal() const;

    template<class F, class ... Args>
    Stage(Signal s, F f, Args ... args)
    {
        signal = s;
        func = bind(f,_1,args...);
    }

private:
    StageType func;
    Signal signal;
};

#endif // STAGE_H
