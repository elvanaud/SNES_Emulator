#include "Stage.h"
#include "W65816.h"

using namespace std::placeholders;


/*template<class F, class ... Args>
Stage::Stage(Signal s, F f, Args ... args)
{
    func = bind(f,_1,args...);
}*/

//template<class F, class ... Args>
std::function<void(W65816*)> Stage::get()
{
    return func;
}

/*void Stage::operator()(W65816 * cpu)
{
    func(cpu);
}*/
