#include "Stage.h"
#include "W65816.h"

using namespace std::placeholders;

std::function<void(W65816*)> Stage::get()
{
    return func;
}

Stage::Signal Stage::getSignal()
{
    return signal;
}
