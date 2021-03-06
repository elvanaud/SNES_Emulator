#include "Stage.h"
#include "W65816.h"

using namespace std::placeholders;

std::function<void(W65816*)> Stage::get() const
{
    return func;
}

Stage::EnablingCondition Stage::getEnablingCondition() const
{
    return enablingCondition;
}
