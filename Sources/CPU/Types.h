#ifndef TYPES_H
#define TYPES_H

#include <functional>

class W65816;
using StageType = std::function<void(W65816*)>;

#endif // TYPES_H

