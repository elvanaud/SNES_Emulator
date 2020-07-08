#ifndef STAGE_H
#define STAGE_H

#include <functional>

#include "Types.h"

class W65816;
using namespace std::placeholders;

class Stage
{
public:
    enum Signal{SIG_ALWAYS};

    StageType get();

    template<class F, class ... Args>
    Stage(Signal s, F f, Args ... args)
    {
        func = bind(f,_1,args...);
    }

private:
    StageType func;
};

#endif // STAGE_H
