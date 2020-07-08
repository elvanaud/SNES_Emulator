#include <iostream>

#include "W65816.h"
#include "Bus.h"

using std::cout;
using std::endl;

int main()
{
    cout << "SNES Emulator - ArthurTendo" << endl;

    W65816 cpu;
    Bus bus(cpu);
    cpu.attachBus(&bus);

    bus.run();

    return 0;
}
