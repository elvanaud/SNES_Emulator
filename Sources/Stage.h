#ifndef STAGE_H
#define STAGE_H

#include <functional>

class W65816;
using namespace std::placeholders;

class Stage
{
public:
    enum Signal{SIG_ALWAYS};

    std::function<void(W65816*)> get();

    template<class F, class ... Args>
    Stage(Signal s, F f, Args ... args)
    {
        func = bind(f,_1,args...);
    }

private:
    std::function<void(W65816*)> func;
};

#endif // STAGE_H
